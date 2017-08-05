#include "include/ServerConfig.h"

//Se leen los parametros de configuracion del servidor desde el archivo
ServerConfig::ServerConfig(char *file)
{  char buf[256], a[256], b[256], c[256];
   char dato[256];
   FILE* conf=fopen(file,"rt");
   
   //Listen Port
   strcpy(dato,"");
   while(!feof(conf))
   {  fgets(buf, 256, conf);
      if (strcmp(buf, "") && buf[0]!=10 && buf[0]!='#')
      {  sscanf(buf, "%s%s%s", a, b, c);
         if (!strcmp(a, "Port"))
         {  strcpy(dato, c);
            dato[strlen(dato)-1] = ' ';
            break;
         }
      }
   }
   listen_port=atoi(dato);
   rewind(conf);
   
   //MaxPlayers
   strcpy(dato,"");
   while(!feof(conf))
   {  fgets(buf, 256, conf);
      if (strcmp(buf, "") && buf[0]!=10 && buf[0]!='#')
      {  sscanf(buf, "%s%s%s", a, b, c);
         if (!strcmp(a, "MaxPlayers"))
         {  strcpy(dato, c);
            dato[strlen(dato)-1] = ' ';
            break;
         }
      }
   }
   max_players=atoi(dato);
   rewind(conf);   

   //Initial Funds
   strcpy(dato,"");
   while(!feof(conf))
   {  fgets(buf, 256, conf);
      if (strcmp(buf, "") && buf[0]!=10 && buf[0]!='#')
      {  sscanf(buf, "%s%s%s", a, b, c);
         if (!strcmp(a, "InitialFunds"))
         {  strcpy(dato, c);
            dato[strlen(dato)-1] = ' ';
            break;
         }
      }
   }
   initial_funds=atoi(dato);
   rewind(conf);

   //Budget rate
   strcpy(dato,"");
   while(!feof(conf))
   {  fgets(buf, 256, conf);
      if (strcmp(buf, "") && buf[0]!=10 && buf[0]!='#')
      {  sscanf(buf, "%s%s%s", a, b, c);
         if (!strcmp(a, "BudgetRate"))
         {  strcpy(dato, c);
            dato[strlen(dato)-1] = ' ';
            break;
         }
      }
   }
   budget=atoi(dato) * initial_funds;
   rewind(conf);
 
   //Bet Time
   strcpy(dato,"");
   while(!feof(conf))
   {  fgets(buf, 256, conf);
      if (strcmp(buf, "") && buf[0]!=10 && buf[0]!='#')
      {  sscanf(buf, "%s%s%s", a, b, c);
         if (!strcmp(a, "BetTime"))
         {  strcpy(dato, c);
            dato[strlen(dato)-1] = ' ';
            break;
         }
      }
   }
   bet_time=atoi(dato);
   rewind(conf);

   //Max Bet
   strcpy(dato,"");
   while(!feof(conf))
   {  fgets(buf, 256, conf);
      if (strcmp(buf, "") && buf[0]!=10 && buf[0]!='#')
      {  sscanf(buf, "%s%s%s", a, b, c);
         if (!strcmp(a, "MaxBet"))
         {  strcpy(dato, c);
            dato[strlen(dato)-1] = ' ';
            break;
         }
      }
   }
   max_bet=atoi(dato);
   rewind(conf);   
 
   fclose(conf);
}
