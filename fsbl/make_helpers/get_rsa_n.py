#!/usr/bin/env python3
# PYTHON_ARGCOMPLETE_OK

import sys
import hashlib
import rsa


def main():
    with open(sys.argv[1], "rb") as ifp:
        din = ifp.read()

    try:
        priv = rsa.PrivateKey.load_pkcs1(din)
    except ValueError:
        priv = rsa.PrivateKey.load_pkcs1(din, format="DER")
    x = rsa.common.byte_size(priv.n)
    n = priv.n.to_bytes(x, byteorder="big")
    digest = hashlib.sha256(n).digest()

    if sys.argv[2] == "-":
        sys.stdout.buffer.write(digest)
    else:
        with open(sys.argv[2], "wb") as ofp:
            ofp.write(digest)


if __name__ == "__main__":
    main()
