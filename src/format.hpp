#pragma once

namespace format {
  template <typename T>
  struct json {
    const T & value;

    json(const T & value) : value{value} {}

    template <typename S>
    friend S & operator<<(S & stream, const json & self) {
      return self.value.json(stream);
    }
  };

  template <typename T>
  struct tsv {
    const T & value;

    tsv(const T & value) : value{value} {}

    template <typename S>
    friend S & operator<<(S & stream, const tsv & self) {
      return self.value.tsv(stream);
    }
  };
}
