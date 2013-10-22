#include <defs.h>

#define DEBUG 0
#define INFO 1
#define ERROR 2
#define VIDEO_MEMORY 0xffffffff800b8000

void puts(int region, char *str);
void putchar(int region, char ch);
char* int_to_str(int integer, char *str, int base);
char* address_to_str(unsigned long address,char *str);
void roll_screen_up();
void print_region(int region, const char* str,__builtin_va_list);

int debug_enabled = 0;
int info_enabled = 1;
int error_enabled = 1;

int y[3] = {0,24,24}, x[3] = {0,130,124};
const int VGA_WIDTH = 160;
const int VGA_HEIGHT = 23;

void printf(const char* str,...){
        __builtin_va_list args;
	__builtin_va_start(args,str);
	print_region(0,str,args);
}

void printTimer(const char* str,...){
        __builtin_va_list args;
	__builtin_va_start(args,str);
	print_region(1,str,args);
	y[1] = 24;
	x[1] = 130;
}

void printKb(const char* str,...){
        __builtin_va_list args;
	__builtin_va_start(args,str);
	print_region(2,str,args);
	x[2]=124;
	y[2]=24;
}

void debug(const char* str,...){
	if(debug_enabled){
	        __builtin_va_list args;
		__builtin_va_start(args,str);
		print_region(0,str,args);
	}	
}

void info(const char* str,...){
	if(info_enabled){
	        __builtin_va_list args;
		__builtin_va_start(args,str);
		print_region(0,str,args);
	}	
}

void error(const char* str,...){
	if(error_enabled){
	        __builtin_va_list args;
		__builtin_va_start(args,str);
		print_region(0,str,args);
	}	
}
void print_region(int region, const char* str,__builtin_va_list args){
	char *mystr;
	char buff[100];
	while(*str != '\0'){
		if(*str == '%'){
			str++;
			switch(*str){
				case 'c':
					putchar(region, __builtin_va_arg(args,int));
					break;
				case 'd':
					mystr = int_to_str(__builtin_va_arg(args,int),buff,10);
					puts(region, mystr);
					break;
				case 's':
					puts(region, __builtin_va_arg(args,char*));
					break;
				case 'x':
					mystr = int_to_str(__builtin_va_arg(args,int),buff,16);
					puts(region, mystr);
					break;
				case 'p':
					mystr = address_to_str(__builtin_va_arg(args,unsigned long),buff);
					puts(region, mystr);
					break;
				default:	
					putchar(region, *(--str));//print %
					break;
			}
		}
		else if(*str == '\n'){
		        x[0]=0;
			y[0]+=1;
			if(y[0]==VGA_HEIGHT){
				 roll_screen_up();
			}

	        }
		else{
		        putchar(region, *str);
			
		}
		str++;
	}	
	
	__builtin_va_end(args);
}

void puts(int region,char* str){
	while(*str){
	        putchar(region,*str);
		str+=1;
	}
        return ;
}

void putchar(int region,char ch){
         volatile register char* startaddress = (char*)VIDEO_MEMORY;
         volatile register char* address;
	 if(region==0){
	  address = startaddress+y[0]*VGA_WIDTH+x[0];
	  *address=ch;
	  x[0]+=2;
	  if(x[0]==VGA_WIDTH){
	   x[0]=0;
	   y[0]+=1;
           if(y[0]==VGA_HEIGHT){
  		 roll_screen_up();	    
	   }
	  } 
	 }
	 else if(region==1){
	  address = startaddress+y[1]*VGA_WIDTH+x[1];
	  *address=ch;
	  x[1]+=2;
	 }
	 else{
	  address = startaddress+y[2]*VGA_WIDTH+x[2];
	  *address=ch;
	  x[2]+=2;
	 }
	 return ;
}

void roll_screen_up(){
	int i;
	volatile register char* startaddress = (char*)VIDEO_MEMORY;
	for(i = 0;i<VGA_WIDTH*(VGA_HEIGHT);i++){
		*(startaddress+i) = *(startaddress+VGA_WIDTH+i);	
	}
	y[0]--;
}

extern void clrscr(){
        volatile register char* startaddress = (char*)VIDEO_MEMORY;
	volatile register char* address;
	for(y[0]=0;y[0]<VGA_HEIGHT+1;y[0]++){
	  for(x[0]=0;x[0]<VGA_WIDTH;x[0]+=1){
	    address=startaddress+y[0]*VGA_WIDTH+x[0];
	    *address=' ';
	    x[0]++;
	    address=startaddress+y[0]*VGA_WIDTH+x[0];
	    *address=0x1F;
	  }
	}  
        x[0]=0;
	y[0]=0;
	for(x[1]=130,y[1]=24;x[1]<VGA_WIDTH;x[1]++){
		x[1]++;
	        address=startaddress+y[1]*VGA_WIDTH+x[1];
		*address=0x34;

	}
	x[1]=130;
	y[1]=24;
	for(x[2]=0,y[2]=24;x[2]<130;x[2]++){
		x[2]++;
	        address=startaddress+y[2]*VGA_WIDTH+x[2];
		*address=0x35;
		
	}
	x[2]=124;
	y[2]=24;
}

char* int_to_str(int integer, char *str, int base){
	char* base_conv_str = "0123456789abcdef";
	char* temp;
	char* num_start;
	temp = str;
	char temp_char;
	int strlen = 0,i;
	//prepend -ve sign
	if(integer < 0){
		*str++ = '-';
		integer = -integer;
	}
	//print prefix 0x for hex numbers/addresses
	if(base == 16){
		*str++ = '0';
		*str++ = 'x';
	}
	num_start = str;//store the place the number reresentation starts!
	do{
		*str++ = base_conv_str[integer%base];
		integer /= base;
		strlen++;
	} while(integer);
	*str = '\0';
	for(i=0;i<strlen/2;i++){
		temp_char = *(num_start+i);
		*(num_start+i)=*(num_start+strlen-i-1);
		*(num_start+strlen-i-1) = temp_char;
		
	}
	return temp;
}

char* address_to_str(unsigned long hexnumber,char *hex_str){
	int i;
	char buff[20];
	int low_half_byte;
	hex_str[0]='0';
	hex_str[1]='x';
	for(i=17;i>=2;i--){
		low_half_byte = hexnumber & 0xf;
		hex_str[i] = int_to_str(low_half_byte,buff,16)[2];
		hexnumber = hexnumber >> 4;
	}
	hex_str[18]='\0';
	return hex_str;
}

void set_debug_level(int debug_level){
	if(debug_level == DEBUG){	
		debug_enabled = 1;
		info_enabled = 1;
	} else if ( debug_level == INFO){
		debug_enabled = 0;
		info_enabled = 1;
	} else if (debug_level == ERROR){
		debug_enabled = 0;
		info_enabled = 0;
	}
}

