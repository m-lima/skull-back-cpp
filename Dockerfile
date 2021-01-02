FROM debian:stable-slim

WORKDIR /opt/skull

# Installs
RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive apt-get install -y \
        git \
        cmake \
        g++ \
        zlib1g-dev \
        python3-pip && \
    pip3 install conan

# Environment
RUN mkdir build && \
    mkdir -p ~/.conan/profiles && \
    echo "[settings]\n\
          os=Linux\n\
          os_build=Linux\n\
          arch=x86_64\n\
          arch_build=x86_64\n\
          compiler=gcc\n\
          compiler.version=8.3\n\
          compiler.libcxx=libstdc++11\n\
          build_type=Release\n\
          [options]\n\
          [build_requires]\n\
          [env]\n" > ~/.conan/profiles/default

# Dependencies
RUN git clone https://github.com/m-lima/mfl.git /opt/mfl && \
    cd /opt/mfl && \
    conan create . mfl/stable --build=missing

# Conan dependencies
COPY conanfile.txt .

RUN cd build && \
    conan install .. --build missing

COPY . .

RUN cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release .. && \
    make

# Slim down
FROM debian:stable-slim

WORKDIR /opt/skull

COPY --from=0 /opt/skull/build/bin/skull /opt/skull/skull

EXPOSE 80

ENTRYPOINT [ "./skull" ]
CMD [ "-h", "", "-t", "1", "-p", "80" ]
