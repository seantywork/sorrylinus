#include "sorrylinus/sock/v1.h"

SSL_CTX* ctx = NULL;
BIO *web = NULL; 
SSL *ssl = NULL;
int SOCK_FD;

const char* const PREFERRED_CIPHERS = "HIGH:!aNULL:!kRSA:!SRP:!PSK:!CAMELLIA:!RC4:!MD5:!DSS";


int hubc_connect(){



    long res = 1;
    int ret = 1;
    unsigned long ssl_err = 0;

    uint8_t rbuff[MAX_BUFF] = {0};
    uint8_t wbuff[MAX_BUFF] = {0};


    int cert_len = read_file_to_buffer(wbuff, MAX_BUFF, CREDENTIAL_LOCATION);


    do {

        init_openssl_library();
        

        const SSL_METHOD* method = SSLv23_method();
        ssl_err = ERR_get_error();
        
        ASSERT(NULL != method);
        if(!(NULL != method))
        {
            print_error_string(ssl_err, "SSLv23_method");

            ret = -1;

            break; 
        }
        
        ctx = SSL_CTX_new(method);

        ssl_err = ERR_get_error();
        
        ASSERT(ctx != NULL);
        if(!(ctx != NULL))
        {
            print_error_string(ssl_err, "SSL_CTX_new");

            ret = -1;

            break; 
        }
        

        SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, verify_callback);

        SSL_CTX_set_verify_depth(ctx, 5);
        

        const long flags = SSL_OP_ALL | SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 | SSL_OP_NO_COMPRESSION;
        long old_opts = SSL_CTX_set_options(ctx, flags);
        UNUSED(old_opts);
        

        res = SSL_CTX_load_verify_locations(ctx, VERIFICATION_LOCATION, NULL);
        ssl_err = ERR_get_error();
        
        ASSERT(1 == res);
        if(!(1 == res))
        {
 
            print_error_string(ssl_err, "SSL_CTX_load_verify_locations");

            ret = -1;

            break; 
        }
        
 
        ssl = SSL_new(ctx); 
        ssl_err = ERR_get_error();

        ASSERT(ssl != NULL);
        if(!(ssl != NULL))
        {
            print_error_string(ssl_err, "SSL_new");
            
            ret = -1;
            
            break; 
        }

        res = SSL_set_cipher_list(ssl, PREFERRED_CIPHERS);
        ssl_err = ERR_get_error();
        
        ASSERT(1 == res);
        if(!(1 == res))
        {
            print_error_string(ssl_err, "SSL_set_cipher_list");

            ret = -1;

            break; 
        }



        int sockfd = get_socket(HOST_FULL_ADDRESS);

        if(sockfd < 0){

            printf("failed to get socket: %d\n", sockfd);

            return -10;
        }

        SSL_set_fd(ssl, sockfd);

        SOCK_FD = sockfd;


        if ( SSL_connect(ssl) != 1 ){
            printf("failed to enable tls connection\n");
        } else {
            printf("enabled tls connection \n");
        }
        
        X509* cert = SSL_get_peer_certificate(ssl);
        if(cert) { X509_free(cert); } 
        
        ASSERT(NULL != cert);
        if(NULL == cert)
        {
  
            print_error_string(X509_V_ERR_APPLICATION_VERIFICATION, "SSL_get_peer_certificate");
            
            ret = -1;
            
            break; 
        }

        res = SSL_get_verify_result(ssl);
        
        ASSERT(X509_V_OK == res);
        if(!(X509_V_OK == res))
        {

            print_error_string((unsigned long)res, "SSL_get_verify_results");
            
            ret = -1;
            
            break;
        }
        

    } while(0);



    if (ret < 0){
        
        printf("failed to connect\n");

        return ret;

    }

    printf("connected\n");


    printf("sending auth\n");


    int send_res = hubc_write(cert_len, wbuff);

    if(send_res <= 0){

        printf("failed to send\n");

        return -1;
    }

    int read_res = hubc_read(MAX_BUFF, rbuff);

    if(read_res <= 0){

        printf("failed to read\n");

        return -1;
    }

    printf("read: %s\n", rbuff);

    return 0;
}


void init_openssl_library(void)
{

    (void)SSL_library_init();

    
    SSL_load_error_strings();

    CONF_modules_load(NULL, NULL, CONF_MFLAGS_IGNORE_MISSING_FILE);

#if defined (OPENSSL_THREADS)
    /* https://www.openssl.org/docs/crypto/threads.html */
    fprintf(stdout, "Warning: thread locking is not implemented\n");
#endif
}

void print_cn_name(const char* label, X509_NAME* const name)
{
    int idx = -1, success = 0;
    unsigned char *utf8 = NULL;
    
    do
    {
        if(!name) break; /* failed */
        
        idx = X509_NAME_get_index_by_NID(name, NID_commonName, -1);
        if(!(idx > -1))  break; /* failed */
        
        X509_NAME_ENTRY* entry = X509_NAME_get_entry(name, idx);
        if(!entry) break; /* failed */
        
        ASN1_STRING* data = X509_NAME_ENTRY_get_data(entry);
        if(!data) break; /* failed */
        
        int length = ASN1_STRING_to_UTF8(&utf8, data);
        if(!utf8 || !(length > 0))  break; /* failed */
        
        fprintf(stdout, "  %s: %s\n", label, utf8);
        success = 1;
        
    } while (0);
    
    if(utf8)
        OPENSSL_free(utf8);
    
    if(!success)
        fprintf(stdout, "  %s: <not available>\n", label);
}

void print_san_name(const char* label, X509* const cert)
{
    int success = 0;
    GENERAL_NAMES* names = NULL;
    unsigned char* utf8 = NULL;
    
    do
    {
        if(!cert) break; /* failed */
        
        names = X509_get_ext_d2i(cert, NID_subject_alt_name, 0, 0 );
        if(!names) break;
        
        int i = 0, count = sk_GENERAL_NAME_num(names);
        if(!count) break; /* failed */
        
        for( i = 0; i < count; ++i )
        {
            GENERAL_NAME* entry = sk_GENERAL_NAME_value(names, i);
            if(!entry) continue;
            
            if(GEN_DNS == entry->type)
            {
                int len1 = 0, len2 = -1;
                
                len1 = ASN1_STRING_to_UTF8(&utf8, entry->d.dNSName);
                if(utf8) {
                    len2 = (int)strlen((const char*)utf8);
                }
                
                if(len1 != len2) {
                    fprintf(stderr, "  Strlen and ASN1_STRING size do not match (embedded null?): %d vs %d\n", len2, len1);
                }
                

                if(utf8 && len1 && len2 && (len1 == len2)) {
                    fprintf(stdout, "  %s: %s\n", label, utf8);
                    success = 1;
                }
                
                if(utf8) {
                    OPENSSL_free(utf8), utf8 = NULL;
                }
            }
            else
            {
                fprintf(stderr, "  Unknown GENERAL_NAME type: %d\n", entry->type);
            }
        }

    } while (0);
    
    if(names)
        GENERAL_NAMES_free(names);
    
    if(utf8)
        OPENSSL_free(utf8);
    
    if(!success)
        fprintf(stdout, "  %s: <not available>\n", label);
    
}

int verify_callback(int preverify, X509_STORE_CTX* x509_ctx)
{
    
    int depth = X509_STORE_CTX_get_error_depth(x509_ctx);
    int err = X509_STORE_CTX_get_error(x509_ctx);
    
    X509* cert = X509_STORE_CTX_get_current_cert(x509_ctx);
    X509_NAME* iname = cert ? X509_get_issuer_name(cert) : NULL;
    X509_NAME* sname = cert ? X509_get_subject_name(cert) : NULL;
    
    fprintf(stdout, "verify_callback (depth=%d)(preverify=%d)\n", depth, preverify);
    

    print_cn_name("Issuer (cn)", iname);
    

    print_cn_name("Subject (cn)", sname);
    
    if(depth == 0) {

        print_san_name("Subject (san)", cert);
    }
    
    if(preverify == 0)
    {
        if(err == X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY)
            fprintf(stdout, "  Error = X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY\n");
        else if(err == X509_V_ERR_CERT_UNTRUSTED)
            fprintf(stdout, "  Error = X509_V_ERR_CERT_UNTRUSTED\n");
        else if(err == X509_V_ERR_SELF_SIGNED_CERT_IN_CHAIN)
            fprintf(stdout, "  Error = X509_V_ERR_SELF_SIGNED_CERT_IN_CHAIN\n");
        else if(err == X509_V_ERR_CERT_NOT_YET_VALID)
            fprintf(stdout, "  Error = X509_V_ERR_CERT_NOT_YET_VALID\n");
        else if(err == X509_V_ERR_CERT_HAS_EXPIRED)
            fprintf(stdout, "  Error = X509_V_ERR_CERT_HAS_EXPIRED\n");
        else if(err == X509_V_OK)
            fprintf(stdout, "  Error = X509_V_OK\n");
        else
            fprintf(stdout, "  Error = %d\n", err);
    }

#if !defined(NDEBUG)
    return 1;
#else
    return preverify;
#endif
}

void print_error_string(unsigned long err, const char* const label)
{
    const char* const str = ERR_reason_error_string(err);
    if(str)
        fprintf(stderr, "%s\n", str);
    else
        fprintf(stderr, "%s failed: %lu (0x%lx)\n", label, err, err);
}


int get_socket(char* url_str){

    int sockfd;
    char hostname[256] = "";
    char    portnum[6] = "";
    char      proto[6] = "";
    char      *tmp_ptr = NULL;
    int           port;
    struct hostent *host;
    struct sockaddr_in dest_addr;


    if(url_str[strlen(url_str)] == '/')
    url_str[strlen(url_str)] = '\0';


    strncpy(proto, url_str, (strchr(url_str, ':')-url_str));

    strncpy(hostname, strstr(url_str, "://")+3, sizeof(hostname));


    if(strchr(hostname, ':')) {
        tmp_ptr = strchr(hostname, ':');

        strncpy(portnum, tmp_ptr+1,  sizeof(portnum));
        *tmp_ptr = '\0';
    }

    port = atoi(portnum);

    if ( (host = gethostbyname(hostname)) == NULL ) {
        printf("failed to get host\n");
        return -1;
    }


    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    dest_addr.sin_family=AF_INET;
    dest_addr.sin_port = htons(port);
    dest_addr.sin_addr.s_addr = *(long*)(host->h_addr);


    memset(&(dest_addr.sin_zero), '\0', 8);

    tmp_ptr = inet_ntoa(dest_addr.sin_addr);


    if ( connect(sockfd, (struct sockaddr *) &dest_addr, sizeof(struct sockaddr)) == -1 ) {

        printf("cannot connect to host\n");

        return -2;
    }

    return sockfd;
}


int hubc_write(int write_len, uint8_t* write_bytes){


    int valwrite = 0;


    SSL* sslfd = ssl;

    valwrite = SSL_write(sslfd, (void*)write_bytes, write_len);

    if (valwrite <= 0){

        printf("write: server gone: %d\n", valwrite);

        return -2;

    }

    return valwrite;


}



int hubc_read(int read_len, uint8_t* read_bytes){


    int valread = 0;

    SSL* sslfd = ssl;

    uint8_t* rbuff_tmp = (uint8_t*)malloc(read_len * sizeof(uint8_t));

    memset(rbuff_tmp, 0, read_len * sizeof(uint8_t));

    int valread_tmp = 0;

    while(valread < read_len){

        valread_tmp = SSL_read(sslfd, (void*)rbuff_tmp, read_len);

        if(valread_tmp <= 0){
            
            if(errno == EAGAIN){

                memset(rbuff_tmp, 0, read_len * sizeof(uint8_t));

                valread_tmp = 0;

                continue;
            }

            printf("read: server gone: %d\n", valread);

            free(rbuff_tmp);

            return -2;
        }

        for(int i = 0 ; i < valread_tmp; i++){

            int idx = valread + i;

            read_bytes[idx] = rbuff_tmp[i];

        }

        valread += valread_tmp;        

        memset(rbuff_tmp, 0, read_len * sizeof(uint8_t));

        valread_tmp = 0;

    }


    free(rbuff_tmp);

    return valread;


}
