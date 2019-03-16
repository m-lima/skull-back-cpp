#include "server.hpp"

#include <fstream>
#include <memory>

#include <mfl/out.hpp>
#include <mfl/string.hpp>
#include <restinio/all.hpp>
#include <restinio/transforms/zlib.hpp>

namespace {
  constexpr const auto GZ_SKULL = "./skull.gz";
  constexpr const auto GZ_QUICK_VALUES = "./quick_values.gz";

  struct ServerTraits : public restinio::default_single_thread_traits_t {
    using request_handler_t = restinio::router::express_router_t<>;
  };

  inline auto createOrOpen(std::string_view path) noexcept {
    std::fstream file;

    file.open(&path.front(), std::ios_base::out | std::ios_base::in);
    if (file.is_open()) {
      file.seekg(0, file.end);
      if (file.tellg() == 0) {
        file << '[';
      } else {
        file.seekg(-1, std::ios_base::end);
        file << ',';
      }
    } else {
      file.open(&path.front(), std::ios_base::app);
      if (file.is_open()) {
        file << '[';
      }
    }

    return file;
  }

//  /* Compress from file source to file dest until EOF on source.
//   def() returns Z_OK on success, Z_MEM_ERROR if memory could not be
//   allocated for processing, Z_STREAM_ERROR if an invalid compression
//   level is supplied, Z_VERSION_ERROR if the version of zlib.h and the
//   version of the library linked do not match, or Z_ERRNO if there is
//   an error reading or writing the files. */
//  int def(FILE *source, FILE *dest, int level)
//  {
//    int ret, flush;
//    unsigned have;
//    z_stream strm;
//    unsigned char in[CHUNK];
//    unsigned char out[CHUNK];
//
//    /* allocate deflate state */
//    strm.zalloc = Z_NULL;
//    strm.zfree = Z_NULL;
//    strm.opaque = Z_NULL;
//    ret = deflateInit(&strm, level);
//    if (ret != Z_OK)
//      return ret;
//
//    /* compress until end of file */
//    do {
//      strm.avail_in = fread(in, 1, CHUNK, source);
//      if (ferror(source)) {
//        (void)deflateEnd(&strm);
//        return Z_ERRNO;
//      }
//      flush = feof(source) ? Z_FINISH : Z_NO_FLUSH;
//      strm.next_in = in;
//
//      /* run deflate() on input until output buffer not full, finish
//         compression if all of source has been read in */
//      do {
//        strm.avail_out = CHUNK;
//        strm.next_out = out;
//        ret = deflate(&strm, flush);    /* no bad return value */
//        assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
//        have = CHUNK - strm.avail_out;
//        if (fwrite(out, 1, have, dest) != have || ferror(dest)) {
//          (void)deflateEnd(&strm);
//          return Z_ERRNO;
//        }
//      } while (strm.avail_out == 0);
//      assert(strm.avail_in == 0);     /* all input will be used */
//
//      /* done when last data in file processed */
//    } while (flush != Z_FINISH);
//    assert(ret == Z_STREAM_END);        /* stream will be complete */
//
//    /* clean up and return */
//    (void)deflateEnd(&strm);
//    return Z_OK;
//  }
//
//  inline void compress() {
//    std::ifstream f;
//    f.open("");
//    f.
//    gzFile output = gzopen("file.gz", "wb");
//    gzwrite(output, "my decompressed data", strlen("my decompressed data"));
//    gzclose(output);
//  }

  inline restinio::transforms::zlib::params_t::format_t
  compressionSupported(restinio::request_handle_t request) noexcept {
    auto compression = request->header().get_field(restinio::http_field::accept_encoding, "");

    if (compression.find("gzip") != std::string::npos) {
      return restinio::transforms::zlib::params_t::format_t::gzip;
    } else if (compression.find("deflate") != std::string::npos) {
      return restinio::transforms::zlib::params_t::format_t::deflate;
    } else {
      return restinio::transforms::zlib::params_t::format_t::identity;
    }
  }

  constexpr inline auto contentEncodingToken(restinio::transforms::zlib::params_t::format_t format) {
    switch (format) {
      case restinio::transforms::zlib::params_t::format_t::gzip:
        return "gzip";
      case restinio::transforms::zlib::params_t::format_t::deflate:
        return "deflate";
      case restinio::transforms::zlib::params_t::format_t::identity:
        return "identity";
    }
  }

  inline restinio::request_handling_status_t forbidden(server::Request request) noexcept {
    return server::fail(request, restinio::status_forbidden());
  }

  inline restinio::request_handling_status_t badRequest(server::Request request) noexcept {
    return server::fail(request, restinio::status_bad_request());
  }

  inline restinio::request_handling_status_t internalServerError(server::Request request) noexcept {
    return server::fail(request, restinio::status_internal_server_error());
  }

  inline restinio::request_handling_status_t notFound(server::Request request) noexcept {
    return server::fail(request, restinio::status_not_found());
  }

  inline restinio::request_handling_status_t sendToLogin(server::Request request) noexcept {
    request->create_response(restinio::status_temporary_redirect())
        .append_header(restinio::http_field::location, "https://auth.mflima.com/login")
        .connection_close()
        .done();
    return restinio::request_accepted();
  }

  template <typename T>
  inline void compressIfAvailable(restinio::response_builder_t<T> response,
                                  restinio::transforms::zlib::params_t::format_t format) noexcept {
    response.append_header(restinio::http_field::content_encoding, contentEncodingToken(format));
  }

}

namespace server {
  void listen(std::string && host,
              std::uint16_t port,
              std::string_view quickValuePath,
              std::string_view skullPath) noexcept {
    auto router = std::make_unique<restinio::router::express_router_t<>>();

    router->http_get("/quick", [quickValuePath](auto request, auto) { return getQuick(request, quickValuePath); });
    router->http_post("/skull", [skullPath](auto request, auto) { return postSkull(request, skullPath); });
    router->http_get("/skull", [skullPath](auto request, auto) { return getSkull(request, skullPath); });
    router->non_matched_request_handler(notFound);

    mfl::out::println("Listening on {:s}:{:d}..", host, port);

    restinio::run(restinio::on_this_thread<ServerTraits>()
                      .address(std::move(host))
                      .port(port)
                      .request_handler(std::move(router)));
  }

  restinio::request_handling_status_t getQuick(Request request, std::string_view quickValuesPath) noexcept {
    if (!authorized(request)) {
      return forbidden(request);
    }

    request->create_response(restinio::status_created())
        .append_header(restinio::http_field::content_type, "text/json; charset=utf-8")
        .set_body(restinio::sendfile(quickValuesPath))
        .done();
    return restinio::request_accepted();
  }

  restinio::request_handling_status_t postSkull(Request request, std::string_view skullPath) noexcept {
    if (!authorized(request)) {
      return forbidden(request);
    }

    const auto query = restinio::parse_query(request->header().query());
    if (!query.has("type") || !query.has("value")) {
      return badRequest(request);
    }

    std::fstream skull = createOrOpen(skullPath);

    if (!skull.is_open()) {
      return internalServerError(request);
    }

    {
      using namespace std::chrono;
      fmt::print(skull,
                 R"-({{"type":"{:s}","value":{:s},"millis":{:d}}}])-",
                 query["type"],
                 query["value"],
                 duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count());
    }
    skull.close();

    // ZLib skull.json

    request->create_response(restinio::status_created()).done();
    return restinio::request_accepted();
  }

  restinio::request_handling_status_t getSkull(Request request, std::string_view skullPath) noexcept {
    if (!authorized(request)) {
      return forbidden(request);
    }

    request->create_response(restinio::status_created())
        .append_header(restinio::http_field::content_type, "text/json; charset=utf-8")
        .set_body(restinio::sendfile(skullPath))
        .done();

    return restinio::request_accepted();
  }

  bool authorized(ConstRequest request) noexcept {
    if (!request->header().has_field(restinio::http_field::cookie)) {
      return false;
    }

    auto cookieHeader = request->header().get_field(restinio::http_field::cookie);
    auto startIndex = cookieHeader.find("session=");

    if (startIndex == std::string::npos) {
      return false;
    }
    startIndex += 8;

    auto endIndex = cookieHeader.find(';', startIndex);
    if (endIndex == std::string::npos) {
      endIndex = cookieHeader.size();
    }

    std::string session{cookieHeader.cbegin() + startIndex, cookieHeader.cbegin() + endIndex};

    if (session.empty()) {
      return false;
    }

    return true;
  }
}
