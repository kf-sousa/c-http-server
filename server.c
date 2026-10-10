#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORTA 8080
#define BUFFER_SIZE 1024

int main(){
    int servidor_fd, novo_socket;
    struct sockaddr_in endereco;
    int opt = 1;
    int addrlen = sizeof(endereco);
    char buffer[BUFFER_SIZE] = {0};

    //criar o descritor do socket (ipv4, TCP)
    if((servidor_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
        perror("Erro ao criar socket");
        exit(EXIT_FAILURE);
    }

    //configurar o socket para reutilizar endereco e porta rapidamente
    if(setsockopt(servidor_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
        perror("Erro em setsockopt");
        exit(EXIT_FAILURE);
    }

    endereco.sin_family = AF_INET;
    endereco.sin_addr.s_addr = INADDR_ANY;
    endereco.sin_port = htons(PORTA);

    //vincular (Bind) o socket a porta e endereco
    if(bind(servidor_fd, (struct sockaddr *)&endereco, sizeof(endereco)) < 0){
        perror("Erro no Bind");
        exit(EXIT_FAILURE);
    }

    //colocar o socket em modo escuta
    if(listen(servidor_fd, 3) < 0){
        perror("Erro no listen");
        exit(EXIT_FAILURE);
    }

    printf("Servidor HTTP rodando na porta %d...\n", PORTA);
    printf("Acesse no navegador: http://localhost:%d\n", PORTA);

    //loop principal do servidor
    while(1){
        //aceitar uma conexao de um cliente
        if((novo_socket = accept(servidor_fd, (struct sockaddr *)&endereco, (socklen_t*)&addrlen)) < 0){
            perror("Erro no accept");
            continue;
        }

        //ler a requisicao enviada pelo navegador
        read(novo_socket, buffer, BUFFER_SIZE);
        printf("--- Requisicao recebida ---\n%s\n", buffer);

        //preparar a resposta http
        char *corpo_html = "<html><body><h1>Olá Servidor C!</h1> <p>Funcionando com sucesso.</p></body></html>";
        char resposta[BUFFER_SIZE];

        snprintf(resposta, sizeof(resposta),
                 "HTTP/1.1 200 OK\r\n"
                 "Content-Type: text/html; charset=UTF-8\r\n"
                 "Content-Length: %ld\r\n"
                 "Connection: close\r\n\r\n"
                 "%s",
                 strlen(corpo_html), corpo_html);

        //enviar a resposta de volta ao cliente
        write(novo_socket, resposta, strlen(resposta));

        //fechar a conexao com este cliente
        close(novo_socket);
        memset(buffer, 0, BUFFER_SIZE);//limpar o buffer para a proxima requisicao
    }

    return 0;

}