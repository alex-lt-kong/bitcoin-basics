#include <crypto++/cryptlib.h>
#include <crypto++/rijndael.h>
#include <crypto++/modes.h>
#include <crypto++/files.h>
#include <crypto++/osrng.h>
#include <crypto++/hex.h>

#include <iostream>
#include <string>

using namespace std;

/* To decrypt this in CryptoJs:

const CryptoJS = require("crypto-js");
var data = "E324B85B5B2CF976F0CFF2E65F45844E";
var _data = CryptoJS.enc.Hex.parse(data);
decrypted = CryptoJS.AES.decrypt(_data.toString(CryptoJS.enc.Base64), CryptoJS.enc.Utf8.parse("LIBMYBITCOIN_KAFKA_32BYTE_ENC_KEY"), {mode:CryptoJS.mode.ECB});
console.log(decrypted.toString(CryptoJS.enc.Utf8));
*/

int main()
{
    using namespace CryptoPP;

    string LIBMYBITCOIN_KAFKA_32BYTE_ENC_KEY=getenv("LIBMYBITCOIN_KAFKA_32BYTE_ENC_KEY");
    SecByteBlock key(reinterpret_cast<const byte*>(
        LIBMYBITCOIN_KAFKA_32BYTE_ENC_KEY.data()),
        LIBMYBITCOIN_KAFKA_32BYTE_ENC_KEY.size());

    string plain = "ECB Mode Test";
    string cipher, encoded, recovered;

    /*********************************\
    \*********************************/

    try
    {
        cout << "plain text: " << plain << endl;

        ECB_Mode< AES >::Encryption e;
        e.SetKey( key, key.size() );

        // The StreamTransformationFilter adds padding
        //  as required. ECB and CBC Mode must be padded
        //  to the block size of the cipher.
        StringSource ss1( plain, true, 
            new StreamTransformationFilter( e,
                new StringSink( cipher )
            ) // StreamTransformationFilter      
        ); // StringSource
    }
    catch( CryptoPP::Exception& e )
    {
        cerr << e.what() << endl;
        exit(1);
    }

    /*********************************\
    \*********************************/

    // Pretty print cipher text
    StringSource ss2( cipher, true,
        new HexEncoder(
            new StringSink( encoded )
        ) // HexEncoder
    ); // StringSource
    cout << "cipher text: " << encoded << endl;

    /*********************************\
    \*********************************/

    try
    {
        ECB_Mode< AES >::Decryption d;
        // ECB Mode does not use an IV
        d.SetKey( key, key.size() );

        // The StreamTransformationFilter removes
        //  padding as required.
        StringSource ss3( cipher, true, 
            new StreamTransformationFilter( d,
                new StringSink( recovered )
            ) // StreamTransformationFilter
        ); // StringSource

        cout << "recovered text: " << recovered << endl;
    }
    catch( CryptoPP::Exception& e )
    {
        cerr << e.what() << endl;
        exit(1);
    }
}