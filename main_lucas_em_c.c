#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <netinet/in.h>
#include <string.h>
#include <dirent.h> 
#include <semaphore.h>
#include <sys/stat.h>

//Lucas Moreira e Silva Alves - [ 20170027336 ]

FILE *fp;
sem_t mutex;
//Funcao que vai retornar o tamanho do arquivo 
long int findSize(char file_name[]) { 

    FILE* fp = fopen(file_name, "r"); 
    if (fp == NULL) { 
        printf("File Not Found!\n"); 
        return -1; 
    } 
    fseek(fp, 0L, SEEK_END); 
    long int res = ftell(fp); 
    fclose(fp); 
    return res; 
} 


//Funcao para encontrar se arquivo existe na pasta Files
int exists(const char *fname)
{
    FILE *file;
    if ((file = fopen(fname, "r")))
    {
        fclose(file);
        return 1;
    }
    return 0;
}

//Funcao principal, sockets, semaforos (para preencher o log.txt), tratamento do GET e HTTP/1.1, checar se o arquivo foi digitado corretamente, expor o arquivo solicitado no browser
//Tudo isso é tratado nessa funcao handle_client
//==================================================================================================================================================================

void* handle_client(void* vsd) {
  int o = 0; 
  //Entrar na pasta onde estao os arquivos requisitados com que serao recuperados com o GET  
  struct dirent *de; 
  chdir("/home/lucosa/Documents/Uni/LP2/20170027336_Lucas_Moreira-LP2-Trabalho2/Files");
  DIR *dr = opendir("."); 
  int count = 0;
  if (dr == NULL) { 
    	printf("Could not open current directory" ); 
    	return 0;
  } 
  else {
		printf("\nEstou no diretorio files\n");
  }
  while ((de = readdir(dr)) != NULL){
        if(de->d_name[0] != '.'){
            printf("%s\n", de->d_name); 
        }
  }
  //--------------------------------------------------------------
  

  //IP string
  //--------------------------------------------------------------
  struct sockaddr_in* pV4Addr = (struct sockaddr_in*)&inet_addr;
  struct in_addr ipAddr = pV4Addr->sin_addr;
  char str[INET_ADDRSTRLEN];
  inet_ntop( AF_INET, &ipAddr, str, INET_ADDRSTRLEN );

  int y = 0, z = 0; 
  y = 0;
  z = 0;

  printf(" [ User IP: %s ]\n", str);
  char respIP[20];
  while (str[z] != '\0'){
      respIP[y] = str[z];
      z++;
      y++;
  }
  //--------------------------------------------------------------

  char respOK[100] = "HTTP/1.1 404 Not Found MOK\nContent-Type: text/html\n\n\n404 Not Found!\n\0";
  char resp[100] = "HTTP/1.1 404 Not Found MOK\nContent-Type: text/html\n\n\n404 Not Found!\n\0";
  char respmf[100] = "HTTP/1.1 404 Not Found MOK\nContent-Type: text/html\n\n\n400 Bad Request!\n\0";

  int  nBytes;
  struct sockaddr_in serverAddr, clientAddr;
  struct sockaddr_storage serverStorage;
  socklen_t addr_size, client_addr_size;
  int i;
  int j = 0, q = 0, k = 0, p = 0, l = 0;
  int flagHTTP = 0;
  char *ext;
  char nome_arquivo[30];
  char arquivo_sem_ext[30];
  char extensao_do_arquivo[5];
  time_t rawtime;
  time (&rawtime);
  struct tm * timeinfo;
  timeinfo = localtime(&rawtime);
  char espaco[6] = "     ";
  char buffer[450];
  char buffertienpo[19];
  strftime(buffertienpo,80,"%x - %I:%M%p", timeinfo);
  printf("Date: |%s|\n", buffertienpo );
  
  /*if((buffer[0] == '\0')){
      printf("Adeus\n");
      return EXIT_SUCCESS;
  }*/
  
  int sd = (int)vsd;
  printf("connected %d\n",sd);
  if(sd == -1){
      return EXIT_SUCCESS;
  }
  recv(sd, buffer, sizeof(buffer),0);
  printf("\nSOU O BUFFER \n- %s -\n", buffer);

//Checar se o GET esta escrito errado
//----------------------------------------------------------------------------------------------------------------------------------
  if (((buffer[0])!='G') || ((buffer[1])!='E') || ((buffer[2])!='T')) {

        //Ativando o mutex para escrever no log e no browser de forma individual
        //------------------------------------------------------------------------------------------------------------------------------------------------
            sem_wait(&mutex);

            char resp[100] = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n\n\n\0";
            char resp2[200] = "<html><strong><head><title>Mini Servidor HTTP</title></head><body><b> Houve um erro na requisicao, cheque o log.txt (arquivo de resposta ao requisito do usuario)! <b/><b/></body></strong></html>\n";
            send(sd,resp,sizeof(resp),0);
            send(sd,resp2,sizeof(resp2),0);
            

            strftime(buffertienpo,80,"%x - %I:%M%p", timeinfo);
            printf("Date: |%s|\n", buffertienpo );   

            printf("Estou na secao critica do semaforo o/");
            fp = fopen("/home/lucosa/Documents/Uni/LP2/20170027336_Lucas_Moreira-LP2-Trabalho2/log.txt", "a");
            fprintf(fp, "\n==============Start Requisition==============\n");
            fprintf(fp, "HTTP/1.1 400 Bad Request\n");
            fprintf(fp, "Date: %s\n", buffertienpo);
            fprintf(fp, "Server: Tomahawk 1.3.3.7 (Win32)");
            fprintf(fp, "%User IP: %s\n", respIP);
            fprintf(fp, "Content-Type: txt/html\n");
            fprintf(fp, "Connection: Closed");
            fprintf(fp, "\n==============End Requisition==============\n");
            fclose(fp);
            sem_post(&mutex);
         //------------------------------------------------------------------------------------------------------------------------------------------------

  }else { 	
	printf("recv buffer: %s\n",buffer);
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------



//Separar o nome do arquivo do buffer, pego atraves da entrada do usuario no browser (127.0.0.1/8080/"arquivo.html")
//Nessa parte tambem eh tirada a extensao do arquivo, o nome separado da extensao, e o HTTP/1.1 tambem eh tratado
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  while(buffer[j] != ' ') {
      j++;
  }

  j+=2;

  while(buffer[j]!= ' ') {
      nome_arquivo[k] = buffer[j];
      j++;
      k++;
  }
  j+=1;
  nome_arquivo[k] = '\0';


//Checar se o HTTP/1.1 esta escrito errado
  if (((buffer[j])!= 'H')|| ((buffer[j+1])!= 'T')|| ((buffer[j+2])!= 'T')|| ((buffer[j+3])!= 'P')|| ((buffer[j+4])!= '/')|| ((buffer[j+5])!= '1')|| ((buffer[j+6])!= '.')|| ((buffer[j+7])!= '1')){
      
    //Ativando o mutex para escrever no log e no browser de forma individual
    //------------------------------------------------------------------------------------------------------------------------------------------------
    sem_wait(&mutex);
      printf("Nao esta no padrao HTTP/1.1\n");
      strftime(buffer,80,"%x - %I:%M%p", timeinfo);
   	  printf("Date: |%s|\n", buffer );

      char resp[100] = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n\n\n\0";
  	  char resp2[200] = "<html><strong><head><title>Mini Servidor HTTP</title></head><body><b> Houve um erro na requisicao, cheque o log.txt (arquivo de resposta ao requisito do usuario)! <b/><b/></body></strong></html>\n";
  	  send(sd,resp,sizeof(resp),0);
	  send(sd,resp2,sizeof(resp2),0);

        printf("Estou na secao critica do semaforo o/");
        fp = fopen("/home/lucosa/Documents/Uni/LP2/20170027336_Lucas_Moreira-LP2-Trabalho2/log.txt", "a");
        fprintf(fp, "\n==============Start Requisition==============\n");
        fprintf(fp, "HTTP/1.1 400 Bad Request\n");
        fprintf(fp, "Date: %s\n", buffertienpo);
        fprintf(fp, "Server: Tomahawk 1.3.3.7 (Win32)\n");
        fprintf(fp, "User IP: %s\n", respIP);
        fprintf(fp, "Content-Type: txt/html\n");
        fprintf(fp, "Connection: Closed\n");
        fprintf(fp, "\n==============End Requisition==============\n");
        fclose(fp);
    sem_post(&mutex);
    //------------------------------------------------------------------------------------------------------------------------------------------------
  }
  
  else if (((buffer[j]) == 'H')|| ((buffer[j+1]) == 'T')|| ((buffer[j+2]) == 'T')|| ((buffer[j+3]) == 'P')|| ((buffer[j+4]) == '/')|| ((buffer[j+5]) == '1')|| ((buffer[j+6]) == '.')|| ((buffer[j+7]) == '1')){
      printf("Esta no padrao HTTP/1.1\n");
  }


  printf("Nome do arquivo: %s\n", nome_arquivo);
  
  while(nome_arquivo[p] != '.'){
      arquivo_sem_ext[l] = nome_arquivo[p];
      p++;
      l++;
  }
  
  arquivo_sem_ext[l] = '\0';
  printf("Arquivo sem extensao: %s\n", arquivo_sem_ext); 

  p+=1;

  while(nome_arquivo[p] != '\0'){
        extensao_do_arquivo[q] = nome_arquivo[p];
        p++;
        q++;     
  }

  extensao_do_arquivo[q]= '\0';    

  printf("Extensao do arquivo: %s\n", extensao_do_arquivo);
//-----------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------


chdir("/home/lucosa/Documents/Uni/LP2/20170027336_Lucas_Moreira-LP2-Trabalho2/Files");

//Tratamento dos arquivos do tipo JPG
//-----------------------------------------------------------------------------------------------------------------------------------
if(strcmp(extensao_do_arquivo, "jpg") == 0) {
    
        if(exists(nome_arquivo) == 1){
            //Ativando o mutex para escrever no log e no browser de forma individual
            //------------------------------------------------------------------------------------------------------------------------------------------------
            sem_wait(&mutex);
            //Tratamento da imagem
            FILE *f = fopen(nome_arquivo, "rb");
            fseek(f, 0, SEEK_END);
            long long fsize = ftell(f);
            fseek(f, 0, SEEK_SET);  
            char *stringimg = malloc(fsize + 1);
            stringimg[0] = '\0';
            fread(stringimg, 1, fsize, f);
            fclose(f);
            printf("O VALOR DE FSIZE:  %lld\n", fsize);
            printf("String com o conteudo da imagem: %s\n", stringimg);
            char resp[100] = "HTTP/1.1 200 OK\nServer: BServer\nAccept-Ranges: bytes\nContent-Type: image/jpeg\n\n";
            send(sd,resp,strlen(resp),0);
            send(sd,stringimg,(fsize + 1),0);
            //--------------------------------------------------------------------------------------------------------------------------
            printf("Estou na secao critica do semaforo o/\n");
            fp = fopen("/home/lucosa/Documents/Uni/LP2/20170027336_Lucas_Moreira-LP2-Trabalho2/log.txt", "a");
            fprintf(fp, "\n==============Start Requisition==============\n");
            fprintf(fp,"HTTP/1.1 200 OK\n");
            fprintf(fp, "Server: Tomahawk 1.3.3.7 (Win32)\n");
            fprintf(fp, "Date: %s\n", buffertienpo);
            fprintf(fp, "User IP: %s\n", respIP);
            fprintf(fp, "Content Lenght: %lld Bytes\n", findSize(nome_arquivo));
            fprintf(fp, "Content-Type: image/jpg\n");
            fprintf(fp, "Connection: Closed\n");
            fprintf(fp, "\n==============End Requisition==============\n");
            fclose(fp);
            sem_post(&mutex);

           
        }
        else if(exists(nome_arquivo) == 0){
            //Ativando o mutex para escrever no log e no browser de forma individual
            //------------------------------------------------------------------------------------------------------------------------------------------------
            sem_wait(&mutex);

            char resp[100] = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n\n\n\0";
  	        char resp2[200] = "<html><strong><head><title>Mini Servidor HTTP</title></head><body><b> Houve um erro no nome do arquivo, cheque o log.txt (arquivo de resposta ao requisito do usuario)! <b/><b/></body></strong></html>\n";
  	        send(sd,resp,sizeof(resp),0);
	        send(sd,resp2,sizeof(resp2),0);

            printf("Error, file not found");
            strftime(buffertienpo,80,"%x - %I:%M%p", timeinfo);

            printf("Estou na secao critica do semaforo o/\n");
            fp = fopen("/home/lucosa/Documents/Uni/LP2/20170027336_Lucas_Moreira-LP2-Trabalho2/log.txt", "a");
            fprintf(fp, "\n==============Start Requisition==============\n");
            fprintf(fp, "HTTP/1.1 404 Not Found\n");
            fprintf(fp, "Server: Tomahawk 1.3.3.7 (Win32)\n");
            fprintf(fp, "%s\n", buffertienpo);
            fprintf(fp, "User IP: %s\n", respIP);
            fprintf(fp, "Content-Type: img/jpg\n");
            fprintf(fp, "Connection: Closed\n");
            fprintf(fp, "\n==============End Requisition==============\n");
            fclose(fp);
            sem_post(&mutex);
        }
   }
//----------------------------------------------------------------------------------------------------------------------------------


//Tratamento dos arquivos do tipo HTML
//----------------------------------------------------------------------------------------------------------------------------------
else if(strcmp(extensao_do_arquivo, "html") == 0) {
        printf("Arquivo do tipo HTML\n");
        if(exists(nome_arquivo) == 1){
            //Tratamento do HTML
            //Ativando o mutex para escrever no log e no browser de forma individual
            //------------------------------------------------------------------------------------------------------------------------------------------------
            sem_wait(&mutex);

            FILE *f = fopen(nome_arquivo, "rb");
            fseek(f, 0, SEEK_END);
            long fsize = ftell(f);
            fseek(f, 0, SEEK_SET);  
            char *string = malloc(fsize + 1);
            fread(string, 1, fsize, f);

            char resp[100] = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n\n\n\0";
  	        char resp2[250] = "<html><strong><head><title>Mini Servidor HTTP</title></head><body><b> Servidor ta funcionando! Cheque o terminal e o log.txt (arquivo de resposta ao requisito do usuario)! <b/><b/></body></strong></html>\n";
  	        send(sd,resp,sizeof(resp),0);
	        send(sd,resp2,sizeof(resp2),0);
            send(sd, string, fsize+1, 0);
            string[fsize] = 0;
        //-----------------------------------------------------------------------------------


            printf("Estou na secao critica do semaforo o/\n");
            fp = fopen("/home/lucosa/Documents/Uni/LP2/20170027336_Lucas_Moreira-LP2-Trabalho2/log.txt", "a");
            fprintf(fp, "\n==============Start Requisition==============\n");
            fprintf(fp,"HTTP/1.1 200 OK\n");
            fprintf(fp, "Server: Tomahawk 1.3.3.7 (Win32)\n");
            fprintf(fp, "Date: %s\n", buffertienpo);
            fprintf(fp, "User IP: %s\n", respIP);
            fprintf(fp, "Content Lenght: %lld Bytes\n", findSize(nome_arquivo));
            fprintf(fp, "Content-Type: txt/html\n");
            fprintf(fp, "Connection: Closed\n");
            fprintf(fp, "\n==============End Requisition==============\n");
            fclose(fp);
            sem_post(&mutex);

        }
        
        else if(exists(nome_arquivo) == 0){
            //Ativando o mutex para escrever no log e no browser de forma individual
            //------------------------------------------------------------------------------------------------------------------------------------------------
            sem_wait(&mutex);

            char resp[100] = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n\n\n\0";
  	        char resp2[200] = "<html><strong><head><title>Mini Servidor HTTP</title></head><body><b> Houve um erro no nome do arquivo, cheque o log.txt (arquivo de resposta ao requisito do usuario)! <b/><b/></body></strong></html>\n";
  	        send(sd,resp,sizeof(resp),0);
	        send(sd,resp2,sizeof(resp2),0);

            printf("Error, file not found");
            strftime(buffertienpo,80,"%x - %I:%M%p", timeinfo);

            printf("Estou na secao critica do semaforo o/");
            fp = fopen("/home/lucosa/Documents/Uni/LP2/20170027336_Lucas_Moreira-LP2-Trabalho2/log.txt", "a");
            fprintf(fp, "\n==============Start Requisition==============\n");
            fprintf(fp, "HTTP/1.1 404 Not Found\n");
            fprintf(fp, "Date: %s\n", buffertienpo);
            fprintf(fp, "Server: Tomahawk 1.3.3.7 (Win32)\n");
            fprintf(fp, "User IP: %s\n", respIP);
            fprintf(fp, "Content-Type: txt/html\n");
            fprintf(fp, "Connection: Closed\n");
            fprintf(fp, "\n==============End Requisition==============\n");
            fclose(fp);
            sem_post(&mutex);
            //----------------------------------------------------------------------------------------------------------------------------------
        }
  }
//----------------------------------------------------------------------------------------------------------------------------------
else if(strcmp(extensao_do_arquivo, "txt") == 0) {
        printf("Arquivo do tipo txt\n");
        if(exists(nome_arquivo) == 1){
            //Tratamento do txt
            //Ativando o mutex para escrever no log e no browser de forma individual
            //------------------------------------------------------------------------------------------------------------------------------------------------
            sem_wait(&mutex);

            FILE *f = fopen(nome_arquivo, "rb");
            fseek(f, 0, SEEK_END);
            long fsize = ftell(f);
            fseek(f, 0, SEEK_SET);  
            char *string = malloc(fsize + 1);
            fread(string, 1, fsize, f);

            char resp[100] = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n\n\n\0";
  	        char resp2[250] = "<html><strong><head><title>Mini Servidor HTTP</title></head><body><b> Servidor ta funcionando! Cheque o terminal e o log.txt (arquivo de resposta ao requisito do usuario)! <b/><b/></body></strong></html>\n";
  	        send(sd,resp,sizeof(resp),0);
	        send(sd,resp2,sizeof(resp2),0);
            send(sd, string, fsize+1, 0);
            string[fsize] = 0;
        //-----------------------------------------------------------------------------------


            printf("Estou na secao critica do semaforo o/\n");
            fp = fopen("/home/lucosa/Documents/Uni/LP2/20170027336_Lucas_Moreira-LP2-Trabalho2/log.txt", "a");
            fprintf(fp, "\n==============Start Requisition==============\n");
            fprintf(fp,"HTTP/1.1 200 OK\n");
            fprintf(fp, "Server: Tomahawk 1.3.3.7 (Win32)\n");
            fprintf(fp, "Date: %s\n", buffertienpo);
            fprintf(fp, "User IP: %s\n", respIP);
            fprintf(fp, "Content Lenght: %lld Bytes\n", findSize(nome_arquivo));
            fprintf(fp, "Content-Type: txt\n");
            fprintf(fp, "Connection: Closed\n");
            fprintf(fp, "\n==============End Requisition==============\n");
            fclose(fp);
            sem_post(&mutex);

        }
        
        else if(exists(nome_arquivo) == 0){
            //Ativando o mutex para escrever no log e no browser de forma individual
            //------------------------------------------------------------------------------------------------------------------------------------------------
            sem_wait(&mutex);

            char resp[100] = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n\n\n\0";
  	        char resp2[200] = "<html><strong><head><title>Mini Servidor HTTP</title></head><body><b> Houve um erro no nome do arquivo, cheque o log.txt (arquivo de resposta ao requisito do usuario)! <b/><b/></body></strong></html>\n";
  	        send(sd,resp,sizeof(resp),0);
	        send(sd,resp2,sizeof(resp2),0);

            printf("Error, file not found");
            strftime(buffertienpo,80,"%x - %I:%M%p", timeinfo);

            printf("Estou na secao critica do semaforo o/");
            fp = fopen("/home/lucosa/Documents/Uni/LP2/20170027336_Lucas_Moreira-LP2-Trabalho2/log.txt", "a");
            fprintf(fp, "\n==============Start Requisition==============\n");
            fprintf(fp, "HTTP/1.1 404 Not Found\n");
            fprintf(fp, "Date: %s\n", buffertienpo);
            fprintf(fp, "Server: Tomahawk 1.3.3.7 (Win32)\n");
            fprintf(fp, "User IP: %s\n", respIP);
            fprintf(fp, "Content-Type: txt\n");
            fprintf(fp, "Connection: Closed\n");
            fprintf(fp, "\n==============End Requisition==============\n");
            fclose(fp);
            sem_post(&mutex);
            //----------------------------------------------------------------------------------------------------------------------------------
        }
  }
//----------------------------------------------------------------------------------------------------------------------------------



  close(sd);
  printf("fechei a conexao %d\n",sd);
  closedir (dr);  
  return EXIT_SUCCESS;
}
//==================================================================================================================================================================



int main(int argc, char** argv) {
    sem_init(&mutex, 0, 1); //Criacao dos semaforos que serao usados para tornar individualizada a escrita no log.txt, arquivo de saida dos codigo 
	int port=22222;
	struct sockaddr_in addr;
    pthread_t threads[5];
    int sd;

	if ( argc != 2 ) 
		printf("usage: %s <portnum>\n...Using default port (%d).\n", argv[0], port);
	else { 
		printf("using port: %s\n",argv[1]);
		port = atoi(argv[1]);
	}

	printf("server start\n");

	sd = socket(PF_INET, SOCK_STREAM, 0);
	/* testar se sd > 0 para saber do sucesso da criacao */
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;

    printf("sd value == [%d]\n", sd);

	if ( bind(sd, (struct sockaddr*)&addr, sizeof(addr)) != 0 ){
		printf("Bind failed\n");
		printf("Tente executar de novo e mudar a port\n");
		return EXIT_FAILURE;
	}//Esse erro provavelmente acontecera caso voce rode mais de uma vez na mesma porta, basta trocar a porta e executar o programa normalmente

	if ( listen(sd, 20) != 0 ) {
		printf("Listen error\n");
		return EXIT_FAILURE;
	}

    int tc = 0;

	while (1)
	{	
		printf("aguardando cliente...\n"); //Aguarda o cliente dar a resposta atraves da porta certa do browser com a porta que foi solicitada
		int client = accept(sd, 0, 0);
		
		int nt = pthread_create(&threads[tc++],NULL,handle_client,(void*)client);
	}
    sem_destroy(&mutex); //acabar os semaforos
	return 0;
}

