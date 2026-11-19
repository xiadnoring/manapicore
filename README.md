# <img src="assets/icon.png" alt="logo" style="width:20px;vertical-align:middle; margin-left:10px;"> <span>Manapi Core</span>


## Introduction
HTTP is disabled now

> [!CAUTION]
> This project is in development!!!

## About
![console](/assets/console1.png)

## Installation

To compile this project, you need to install below projects:
- **libuv 1.49.2 or greater \[required\]**
- openssl 3.3.1 or greater \[optional\]
- zlib 1.3.1 or greater \[optional\]
- gmp 6.3.0 or greater \[optional\]
- curl 8.8.0-1 or greater \[optional\]
- wolfssl 5.5.0 or greater \[optional\]
- quiche 0.22.0 or greater \[optional\]
- grpc 1.72.0 or greater \[optional\]
- cpptrace 0.74.1 or greater \[optional\]
- brotli 1.1.0 or greater \[optional\]
- zstd 1.5.5 or greater \[optional\]
- nghttp2 1.59.0 or greater \[optional\]
- nghttp3 1.6.0 or greater \[optional\]

## Build

```bash
cmake -DCMAKE_BUILD_TYPE=Debug/Release ...
```

### Build as Executable
```bash
cmake ... -DMANAPICORE_BUILD_TYPE=exe
```

### Build as Library
```bash
cmake ... -DMANAPICORE_BUILD_TYPE=lib
```

### Build with Conan
```bash
cmake ... -DMANAPICORE_BUILD_METHOD=conan
```

### Build with cmake Fetch
> [!WARNING]
> not everything is supported
> 
> (gmp (linux), libuv, zlib, curl, openssl, nghttp2, nghttp3)
```bash
cmake ... -DMANAPICORE_BUILD_METHOD=fetch
```

## Example

```c++
int main () {
    /* creates 2 threads for blocking I/O syscalls */
    manapi::context::threadpoolfs(2);
    /* disable several signals */
    manapi::context::gbs (manapi::context::blockedsignals());
    /* creates 4 additional threads for 4 additional event loops */
    auto ctx = manapi::context::create(4).unwrap();
    /* metric */
    std::atomic<int> cnt = 0;
    /* runs main event loop and 4 additional event loops */
    ctx->run(4, [&cnt] (std::function<void()> bind) -> void {
        auto db = manapi::ext::pq::db::create().unwrap();

        manapi::async::run([router, db] () mutable -> manapi::future<> {
            auto master = manapi::ext::pq::pool::create().unwrap();
            manapi::unwrap(co_await master->connect(2, "127.0.0.1", "7879", "development", "password", "db"));
            db->set_master(std::move(master)).unwrap();
        });

        /* bind event loop in the current context */
        bind();
    }).unwrap();

    manapi::clear_tools::curl_library_clear();
    manapi::clear_tools::ev_library_clear();
    manapi::clear_tools::ssl_library_clear();
    manapi::clear_tools::grpc_clear();

    return 0;
}
```

## Params

| Option name                         | Description              | Values                  |
|:------------------------------------|:-------------------------|:------------------------|
| MANAPICORE_GMP_DEPENDENCY           | Bigint Support           | **ON**/OFF              |
| MANAPICORE_CURL_DEPENDENCY          | Fetch Support            | **ON**/OFF              |
| MANAPICORE_ZLIB_DEPENDENCY          | gzip/deflate Support     | **ON**/OFF              |
| MANAPICORE_QUICHE_DEPENDENCY        | HTTP3/QUIC by Cloudflare | **ON**/OFF              | 
| MANAPICORE_OPENSSL_DEPENDENCY       | TLS/QUIC by OpenSSL      | **ON**/OFF              |
| MANAPICORE_WOLFSSL_DEPENDENCY       | TLS by WolfSSL           | ON/**OFF**              |
| MANAPICORE_GRPC_DEPENDENCY          | gRPC Support             | **ON**/OFF              |
| MANAPICORE_BROTLI_DEPENDENCY        | brotli Support           | **ON**/OFF              |
| MANAPICORE_ZSTD_DEPENDENCY          | zstd Support             | **ON**/OFF              |
| MANAPICORE_WOLFSSL_WITH_ALPN        | WolfSSL with ALPN        | **ON**/OFF              |
| MANAPICORE_NGHTTP2_DEPENDENCY       | HTTP2 by nghttp2         | **ON**/OFF              |
| MANAPICORE_NGHTTP3_DEPENDENCY       | HTTP3 by nghttp3         | **ON**/OFF              |
| MANAPICORE_CPPTRACE_DEPENDENCY      | CppTrace                 | ON/**OFF**              |
| MANAPICORE_JSON_DEBUG               | JSON debug symbols       | **ON**/OFF              |
| MANAPICORE_STD_BACKTRACE_DEPENDENCY | STD C++23 stacktrace     | **ON**/OFF              |
| BUILD_SHARED_LIBS                   | Build as a shared lib    | ON/**OFF**              |
| MANAPICORE_BUILD_METHOD             | Build Method             | conan/**default**/fetch | 
| MANAPICORE_DEPENDENCY_FETCH_SHARED  | Bsuild shared depends    | ON/**OFF**              |
| MANAPICORE_OPENSSL_SYSTEM_DISABLE   | Disable default OpenSSL  | ON/**OFF**              |
| MANAPICORE_CURL_SYSTEM_DISABLE      | Disable default cURL     | ON/**OFF**              |
| MANAPICORE_DEPENDENCY_FETCH_OUTPUT  | Depends Output Path      | \<PATH\>/**OFF**        |
| MANAPICORE_BUILD_TYPE               | Binary or Library        | exe/**lib**/test        |
| MANAPICORE_INSTALL_ARCH             | '/x86_64-linux-gnu'      | '/\<PATH\>'/**OFF**     |

## Made in

[![Башкортостан](assets/rb2.png)](https://ru.wikipedia.org/wiki/%D0%91%D0%B0%D1%88%D0%BA%D0%BE%D1%80%D1%82%D0%BE%D1%81%D1%82%D0%B0%D0%BD)