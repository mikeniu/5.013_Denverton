@REM
@REM
cd Wpa_supplicant/wpa_supplicant-0.6.10/
copy src/utils/common.h         ../../AmiCryptoPkg/CryptoLib
copy src/utils/os.h             ../../AmiCryptoPkg/CryptoLib
copy src/crypto/crypto.h        ../../AmiCryptoPkg/CryptoLib
copy src/crypto/md5.h           ../../AmiCryptoPkg/CryptoLib
copy src/crypto/md5.c           ../../AmiCryptoPkg/CryptoLib
copy src/crypto/sha1.h          ../../AmiCryptoPkg/CryptoLib
copy src/crypto/sha1.c          ../../AmiCryptoPkg/CryptoLib
copy src/crypto/sha256.h        ../../AmiCryptoPkg/CryptoLib
copy src/crypto/sha256.c        ../../AmiCryptoPkg/CryptoLib
copy src/tls/asn1.h             ../../AmiCryptoPkg/CryptoLib
copy src/tls/asn1.c             ../../AmiCryptoPkg/CryptoLib
copy src/tls/rsa.h              ../../AmiCryptoPkg/CryptoLib
copy src/tls/rsa.c              ../../AmiCryptoPkg/CryptoLib
copy src/tls/bignum.h           ../../AmiCryptoPkg/CryptoLib
copy src/tls/bignum.c           ../../AmiCryptoPkg/CryptoLib
copy src/tls/libtommath.c       ../../AmiCryptoPkg/CryptoLib
copy src/tls/x509.c             ../../AmiCryptoPkg/CryptoLib
copy src/tls/x509.c             ../../AmiCryptoPkg/CryptoLib
cd ..
