
#ifndef PREPROCESSOR_HEADER_SOCK
#define PREPROCESSOR_HEADER_SOCK

#include "./sock_resource.h"

#endif
 
int sig_verify(const char* cert_pem, const char* intermediate_pem)
{
    BIO *b = BIO_new(BIO_s_mem());
    BIO_puts(b, intermediate_pem);
    X509 * issuer = PEM_read_bio_X509(b, NULL, NULL, NULL);
    EVP_PKEY *signing_key=X509_get_pubkey(issuer);
 
    BIO *c = BIO_new(BIO_s_mem());
    BIO_puts(c, cert_pem);
    X509 * x509 = PEM_read_bio_X509(c, NULL, NULL, NULL);
 
    int result = X509_verify(x509, signing_key);
 
    EVP_PKEY_free(signing_key);
    BIO_free(b);
    BIO_free(c);
    X509_free(x509);
    X509_free(issuer);
 
    return result;
}



void cert_info(const char* cert_pem)
{
    BIO *b = BIO_new(BIO_s_mem());
    BIO_puts(b, cert_pem);
    X509 * x509 = PEM_read_bio_X509(b, NULL, NULL, NULL);

    BIO *bio_out = BIO_new_fp(stdout, BIO_NOCLOSE);
 
    BIO_printf(bio_out,"Subject: ");
    X509_NAME_print(bio_out,X509_get_subject_name(x509),0);
    BIO_printf(bio_out,"\n");

    BIO_printf(bio_out,"Issuer: ");
    X509_NAME_print(bio_out,X509_get_issuer_name(x509),0);
    BIO_printf(bio_out,"\n");
 

    //EVP_PKEY *pkey=X509_get_pubkey(x509);
    //EVP_PKEY_print_public(bio_out, pkey, 0, NULL);
    //EVP_PKEY_free(pkey);

    //X509_signature_print(bio_out, x509->sig_alg, x509->signature);
    //BIO_printf(bio_out,"\n");
 
    BIO_free(bio_out);
    BIO_free(b);
    X509_free(x509);
}


std::string check_if_verified_conn(std::map<void*, std::string>* connections,
                            connection_hdl hdl){

  std::string status;

  /*
  for (auto& connection : *connections) {
    if (connection.lock() == hdl.lock()) {
      status = true;
    }
  }
  */

  if (connections->find(hdl.lock().get()) == connections->end()) {
    status = "FAIL";
  } else {
    status = (*connections)[hdl.lock().get()];
  }

  std::cout << "verified connections: " << connections->size() << std::endl;
  std::cout << "verified status: " << status << std::endl;
  return status;
}

std::string authenticate_request(websocketpp::config::asio::message_type::ptr msg){

  std::string ret_c_name = "FAIL";

  bool result = false;

  std::string payload = msg->get_payload();

  const char * cert_pem = payload.c_str();


  std::ifstream in_cert("tls/ca.crt");
  std::stringstream buffer;

  std::string ca_payload;

  buffer << in_cert.rdbuf();

  ca_payload = buffer.str();

  const char * intermediate_pem = ca_payload.c_str();

  int v = sig_verify(cert_pem, intermediate_pem);

  if(v < 1){
    return ret_c_name;
  }


  ret_c_name = extract_common_name(cert_pem);


  return ret_c_name;
}

int test_verify(){


    OpenSSL_add_all_algorithms();
    OpenSSL_add_all_ciphers();
    OpenSSL_add_all_digests(); 
    const char cert[] = "-----BEGIN CERTIFICATE-----" "\n"
"MIIEzDCCArQCFDYtKXyRz7cbmLnlyeo8uVVGMigvMA0GCSqGSIb3DQEBCwUAMCIx" "\n"
"IDAeBgNVBAMMF2lzc3VlcmZvcnNlYW50eXdvcmsuY29tMB4XDTIzMDgyNTEwMDAw" "\n"
"OFoXDTI0MDIyMTEwMDAwOFowIzEhMB8GA1UEAwwYc3ViamVjdGZvcnNlYW50eXdv" "\n"
"cmsuY29tMIICIjANBgkqhkiG9w0BAQEFAAOCAg8AMIICCgKCAgEAred3Rt1mmOwz" "\n"
"HfgXUrKHNn/keHUbu1jFoE/yF0WqToTZLQ1jhgqUh/hmOrZ145rJh9GAPgVLbTnR" "\n"
"fx8VlGxo7PQIPXhltpUbSTCRRMM6UleIcMbBj+6h0/xWoTlWDSgB4byfGyHmQs1q" "\n"
"eEJSdgtr28KFr/gJ+vXUDITqU0ogOfLeYs5nbqjkPqq4J0eoJGDndCqZO5Emgt03" "\n"
"dRr6BpXVSjm7WAZUPxbwOVHXsQh4KowHn7NUfWLC25a2sp5DNUNb5qNAQQPuYR9U" "\n"
"vPlC0fCdTkEIEtoCPD51Gc/k/pmK4pbt8Trf9tYE9VODGZxgayD6yyO0y3FKmBLI" "\n"
"Ccpu6/n9v8Wi3d/tbpNHODXhm2QIE7GI7PfPPC1fNM0q8csGdYPXd7y8CcCyV9z9" "\n"
"hFOL6ox7FFYOZmcb/irPZVXrz7IWtxRr5aerFzgsmJFfQVibqDcWAd1MHkbufxwU" "\n"
"hHQe9SF9+RBKtGNJHAPuUT3SOwwVOCj4fhwA71MOQ3oHT7CppcfEaBXvVuvkdh/W" "\n"
"0grI4uBehsdGlOfkF3UO519dc2ua39ErP/JiG4l5JrHUVSgDZ70z79yAEBISvcFO" "\n"
"w5yXTVbMyHFg4oVyUEvGtrgjWqVjNEvSo0Y+4gC7A0DWEs4RrH7B2L1a/7XnHgik" "\n"
"/FH8spFP7XXCXT2L4SpfeOQTEWdQqfsCAwEAATANBgkqhkiG9w0BAQsFAAOCAgEA" "\n"
"TAslifntjMebuTzzD/M9F0kGFV4QaoIZUwoNcLcvOAlgsZzKTCmJilfYxJR7ZCTI" "\n"
"1SKCizCyo9NoKt7l6PorQeGEQbe8B9XJn2ieMR63S/PAjI73+V+s+l8/Kho/EVu4" "\n"
"p8/c7I9DIFyhNKm3gBhXQt1N3Lk5tGIPH9pBYn8uxgrX3rerGHnTPYBFt2maGTYx" "\n"
"QWk23vDR5HW/kciCs/MzGP16N43FgZUrwdq8ngLUAA3F8bvAbAHdS3jBqdbLipKZ" "\n"
"fgcp8JcaO94OfDJQKdHW94cMlG5qELgZ4R6V98/foqVlM2j5tBMR/uwJFQ2nBZX+" "\n"
"CjQ7w7dwPsFQnRsC+smBQM8T6sdJKm35MAB435bdXBjFH7EMvs1QihYQgP8DviRm" "\n"
"Rz8+UuLg+0lNN/H7dFRxJie3bThHr5JED6/dwE2qVeJ2kVArdfY2c8Ug0kck9FpX" "\n"
"tSwDM6m/4shm+METPfdKfZNQcBhg9c9vqx1kfaXIS/ItUW2aCd/EJVOY3SRZyGpT" "\n"
"tV5eZkPCMfsJGmjXkNjEZO4sHjoZfj/gtm+tN+Wf2cqR2S+8fimaGPFUeU71f0Ly" "\n"
"TMDAl2WqlmjsXrS4MnKg6JWuCH2IB9fYHReyV90kiRTyzkZyuA++H1za0bBicefu" "\n"
"2qZ/Lu70VGswSPmD7ug3EPWxBJO9ssBP91rRKo8AE/o=" "\n"
"-----END CERTIFICATE-----";

    const char intermediate[] = "-----BEGIN CERTIFICATE-----" "\n"
"MIIFJTCCAw2gAwIBAgIUSqTm9qaKHOrxrdj/tzzQlK6eTAAwDQYJKoZIhvcNAQEL" "\n"
"BQAwIjEgMB4GA1UEAwwXaXNzdWVyZm9yc2VhbnR5d29yay5jb20wHhcNMjMwODI1" "\n"
"MDk1NDUwWhcNMjQwODI0MDk1NDUwWjAiMSAwHgYDVQQDDBdpc3N1ZXJmb3JzZWFu" "\n"
"dHl3b3JrLmNvbTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBALmqu1sO" "\n"
"uEG4QmawaD6Rj6N+i8V3RF40q8ULe1Nbrf55irvrzAA4Yx9raUp/qvebvr9zTh/X" "\n"
"UJf4UhlVR8jfTGZmNhBFJGhebwUe8/XQZ0eI9+JxyKWIkXK1kYhpxAqeQH8tCwko" "\n"
"PHjZE0COJvYxAogkE/Es6yL6SZ3lGZsm2W8S9MySm+YkLHCxxY0HPK5YzIq/Pwb+" "\n"
"B4UY5xQcmlQf98wNjW9kxUicSaylsqus8e8Hh8SbvXa5RQyxqMeDuORa4fVTcvpQ" "\n"
"M1cHuJ1OXVKMQ9dXF0gJ/uKctpPbe/YvKHjiUoVwHk/Lu5uBMbD7FMLfSUs1shZE" "\n"
"frb0Ot50SFOAeFW6fp79gmAiTJRNkQPsrwe9pxmxQb6Uwn+luQNQzDM7bItQ9Vzf" "\n"
"abNZ7SK/6duA3HRsVHrkWA1EM/7RrPwo7cKY1xwYEb42+N86LF5Tt1Nx/Sfvp8QG" "\n"
"erryeNQvG9l3Sr0NZHwkjVvxSBu2GQM7FckNi03F98me8U1ljsZ/LvPMW3gO+mem" "\n"
"XfJKN2COL9N3sxQ/+SJoz5L/D9Nj1rmYqnRd/376CL4VQ1el6woT9x2hbL+oMxZF" "\n"
"7uFkGWDs8qF59X/D6sRl3STOIvpFgUjjkO9T58RXL50OUh6YtxIIg57evrrPnb8R" "\n"
"Ogm2Ibj1YjHb1MqgRhJ9NB/e7TkK3l1hxeuFAgMBAAGjUzBRMB0GA1UdDgQWBBQi" "\n"
"QxEHgsuKbNyEQVldf0L77Aj8DTAfBgNVHSMEGDAWgBQiQxEHgsuKbNyEQVldf0L7" "\n"
"7Aj8DTAPBgNVHRMBAf8EBTADAQH/MA0GCSqGSIb3DQEBCwUAA4ICAQAlUm4pPOUO" "\n"
"s+6MEyMT2fdhforIi2cLeTrvakO0MDk6CAHMkvI+wqwEyOOFU45wbwJ5AArV5gNH" "\n"
"c68cj/GG+ddneWXGRfLRprKrrxcvKk4d2Xl9o/BK9R5TAiA0jIwIHlQaCMCfnAO2" "\n"
"CiKA5eCHxYGsY6rAQJvOpng8o3HO70lN/umm9eS+x6SVjh22GZEdifoVrW/nxV5Y" "\n"
"2E9dbZ6hLtL3bJ9H/1KYyi6w2cSA7g2h+Sq7WTwAkPHd/Y97KSFT5ivzdZNGeQtY" "\n"
"qJdt5GdRgnBw5kaT8JfUG4kGW8zmsfT0q//Jg9Mq86d1o8w88luBJIfHAP2pGn7i" "\n"
"cvZjuqkJ6Xu0zWsihZSZinhEqRQbp8vZL+YY6aGtMWbYcoaUgXtqjqkpcz9OqZ4i" "\n"
"VaKQnjbcRvIJBy4IMjXP0w1cB69vomZO6vWBolYvGByV6u8jo66mgbChLMCXr27e" "\n"
"6gcfVYkhgwnQox8ykM8rOwmMpiam+Vq5v5/Bv3CtqEY95TPsKTe+86XMSx+cmIPR" "\n"
"prk8pepCCl/Ny8MwNTgA3IZuBOA6sxaJq/QYJzbP7U8SArOcsIB829mhlg16dGRd" "\n"
"9gaR+KWTDGRgwooZr7XyIDWeY0LAXW09xHlf7DAeh5rK+T9d8TJY6q5Z/xaPGO2Q" "\n"
"ofgjc+izntxq6wTAw1BbIOJGAFa7Q0SpiQ==" "\n"
"-----END CERTIFICATE-----";

    cert_info(cert);
    cert_info(intermediate);
    int res = sig_verify(cert,intermediate);
    printf("result: %d\n",res);

    return res;
}


int test_get_sn(){

      const char cert[] = "-----BEGIN CERTIFICATE-----" "\n"
"MIIEzDCCArQCFDYtKXyRz7cbmLnlyeo8uVVGMigvMA0GCSqGSIb3DQEBCwUAMCIx" "\n"
"IDAeBgNVBAMMF2lzc3VlcmZvcnNlYW50eXdvcmsuY29tMB4XDTIzMDgyNTEwMDAw" "\n"
"OFoXDTI0MDIyMTEwMDAwOFowIzEhMB8GA1UEAwwYc3ViamVjdGZvcnNlYW50eXdv" "\n"
"cmsuY29tMIICIjANBgkqhkiG9w0BAQEFAAOCAg8AMIICCgKCAgEAred3Rt1mmOwz" "\n"
"HfgXUrKHNn/keHUbu1jFoE/yF0WqToTZLQ1jhgqUh/hmOrZ145rJh9GAPgVLbTnR" "\n"
"fx8VlGxo7PQIPXhltpUbSTCRRMM6UleIcMbBj+6h0/xWoTlWDSgB4byfGyHmQs1q" "\n"
"eEJSdgtr28KFr/gJ+vXUDITqU0ogOfLeYs5nbqjkPqq4J0eoJGDndCqZO5Emgt03" "\n"
"dRr6BpXVSjm7WAZUPxbwOVHXsQh4KowHn7NUfWLC25a2sp5DNUNb5qNAQQPuYR9U" "\n"
"vPlC0fCdTkEIEtoCPD51Gc/k/pmK4pbt8Trf9tYE9VODGZxgayD6yyO0y3FKmBLI" "\n"
"Ccpu6/n9v8Wi3d/tbpNHODXhm2QIE7GI7PfPPC1fNM0q8csGdYPXd7y8CcCyV9z9" "\n"
"hFOL6ox7FFYOZmcb/irPZVXrz7IWtxRr5aerFzgsmJFfQVibqDcWAd1MHkbufxwU" "\n"
"hHQe9SF9+RBKtGNJHAPuUT3SOwwVOCj4fhwA71MOQ3oHT7CppcfEaBXvVuvkdh/W" "\n"
"0grI4uBehsdGlOfkF3UO519dc2ua39ErP/JiG4l5JrHUVSgDZ70z79yAEBISvcFO" "\n"
"w5yXTVbMyHFg4oVyUEvGtrgjWqVjNEvSo0Y+4gC7A0DWEs4RrH7B2L1a/7XnHgik" "\n"
"/FH8spFP7XXCXT2L4SpfeOQTEWdQqfsCAwEAATANBgkqhkiG9w0BAQsFAAOCAgEA" "\n"
"TAslifntjMebuTzzD/M9F0kGFV4QaoIZUwoNcLcvOAlgsZzKTCmJilfYxJR7ZCTI" "\n"
"1SKCizCyo9NoKt7l6PorQeGEQbe8B9XJn2ieMR63S/PAjI73+V+s+l8/Kho/EVu4" "\n"
"p8/c7I9DIFyhNKm3gBhXQt1N3Lk5tGIPH9pBYn8uxgrX3rerGHnTPYBFt2maGTYx" "\n"
"QWk23vDR5HW/kciCs/MzGP16N43FgZUrwdq8ngLUAA3F8bvAbAHdS3jBqdbLipKZ" "\n"
"fgcp8JcaO94OfDJQKdHW94cMlG5qELgZ4R6V98/foqVlM2j5tBMR/uwJFQ2nBZX+" "\n"
"CjQ7w7dwPsFQnRsC+smBQM8T6sdJKm35MAB435bdXBjFH7EMvs1QihYQgP8DviRm" "\n"
"Rz8+UuLg+0lNN/H7dFRxJie3bThHr5JED6/dwE2qVeJ2kVArdfY2c8Ug0kck9FpX" "\n"
"tSwDM6m/4shm+METPfdKfZNQcBhg9c9vqx1kfaXIS/ItUW2aCd/EJVOY3SRZyGpT" "\n"
"tV5eZkPCMfsJGmjXkNjEZO4sHjoZfj/gtm+tN+Wf2cqR2S+8fimaGPFUeU71f0Ly" "\n"
"TMDAl2WqlmjsXrS4MnKg6JWuCH2IB9fYHReyV90kiRTyzkZyuA++H1za0bBicefu" "\n"
"2qZ/Lu70VGswSPmD7ug3EPWxBJO9ssBP91rRKo8AE/o=" "\n"
"-----END CERTIFICATE-----";

  std::string res = extract_common_name(cert);

  printf("result: %s\n",res.c_str());

  return 0;
}