conan create .  \
              -o "manapicore/*:shared=True" -o "manapicore/*:json_debug=True" \
              -o "manapicore/*:wolfssl_dependency=False" -o "manapicore/*:openssl_dependency=True" \
              -o "manapicore/*:quiche_dependency=False" -o "manapicore/*:tquic_dependency=False" \
              -o "manapicore/*:curl_dependency=True" -o "manapicore/*:gmp_dependency=False" \
              -o "manapicore/*:zlib_dependency=False" -o "manapicore/*:lib=True" -o "libcurl/*:shared=True" \
              -o "openssl/*:shared=True" -o "libev/*:shared=True" --build=missing