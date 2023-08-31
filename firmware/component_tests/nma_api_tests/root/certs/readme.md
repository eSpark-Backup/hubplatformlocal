# Certificates for component tests

## Local mode certificates for tests update

Updated certificates are valid till: 2023-06-20.

## Update local mode certificates for tests

To start some component tests, the started local mode server running in the ha-uid. service is needed.

To start it, there should be valid local mode certificates. They are generated on cloud side.

To update it, copy files from a real hub: **/home/data/certs/local/** folder.

To check the certificate's due date, please, call:

```sh
openssl x509 -in 0.crt -text -noout
```

Output will contain the following:
```
Certificate:
    Data:
        Version: 1 (0x0)
        Serial Number:
            11:2c:16:75:54:6a:15:cf:99:98:db:12:ba:58:be:1f:61:ab:02:f3
        Signature Algorithm: sha256WithRSAEncryption
        Issuer: C = RO, ST = Iasi, L = Iasi, O = location, OU = dd2c4250-52f2-11ea-a1f2-e3a7b5d5b167, CN = clas.ezlo.com, emailAddress = sysadmins@ezlo.com
        Validity
            Not Before: Jun 25 14:18:04 2022 GMT
            Not After : Dec 22 14:18:04 2022 GMT
...
```

This certificate is valid until 2022-12-22.