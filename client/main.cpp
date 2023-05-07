#include <iostream>
#include <string>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/bio.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <nlohmann/json.hpp>
#include "libs/Weather.h"
#include <curl/curl.h>


using json = nlohmann::json;

int main() {
    // Inicjalizacja OpenSSL
    SSL_library_init();
    SSL_load_error_strings();

    // Tworzenie kontekstu SSL
    SSL_CTX* ctx = SSL_CTX_new(TLS_client_method());

     // Sprawdzanie poprawności certyfikatu
    for (int i = 0; i < 3; i++) {
        if (SSL_CTX_load_verify_locations(ctx, "client.crt", NULL) != 1) 
        {
            std::cerr << "Nie można załadować certyfikatu!" << std::endl;            
        }
    else
        {
            std::cout << "Certyfikat załadowany poprawnie" << std::endl;
            SSL_CTX_use_certificate_file(ctx, "client.crt", SSL_FILETYPE_PEM);
            break;
        }
    }

    
    SSL_CTX_use_PrivateKey_file(ctx, "client.key", SSL_FILETYPE_PEM);
    

    // Tworzenie zapytania do serwera o pogodę w podanym mieście
    std::string city;
    std::cout << "Podaj miasto: ";
    std::cin >> city;
    // sparowanie zapytania do jsona
    json request;
    request["city"] = city;
    std::string requestString = request.dump();   
   

    // Tworzenie gniazda i połączenie z serwerem
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(2137);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);
    connect(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr));
    //sprawdzanie czy połączenie zostało nawiązane
    if (sockfd < 0) 
    {
        std::cerr << "Nie można nawiązać połączenia!" << std::endl;
        return 1;
    }
    else
    {
        std::cout << "Połączenie nawiązane" << std::endl;
    }

    // Tworzenie obiektu SSL
    SSL* ssl = SSL_new(ctx);
    SSL_set_fd(ssl, sockfd);

    // Uwierzytelnianie i przeprowadzanie komunikacji
    if (SSL_connect(ssl) <= 0) {
        ERR_print_errors_fp(stderr);
    } 
    else 
    {
       // wysyłanie do serwera
        SSL_write(ssl, requestString.c_str(), requestString.length());

        // czekanie na odpowiedź
        sleep(1);

        // odbieranie odpowiedzi od serwera
        char buffer[1024] = {0};
        SSL_read(ssl, buffer, 1024);

        
       
        //parsowanie odpowiedzi do jsona
        json response = json::parse(buffer);
        // sprawdzenie czy zwrócony json jest poprawny
        if(response["status"] == "error")
        {
            std::cout << "Nie znaleziono miasta" << std::endl;
            return 0;
        }
        else if (response["status"] == "ok")
        {
            Weather myWeather;
            myWeather.setReceivedWeather(response);
            myWeather.parseServerResponse();    
            std::cout << std::fixed <<  std::setprecision(2) << myWeather.parseServerResponse() << std::endl;

        }
        

    }

    // Zamykanie połączenia i zwalnianie zasobów
    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(sockfd);

    // Zwalnianie zasobów kontekstu SSL
    SSL_CTX_free(ctx);

    return 0;
}
