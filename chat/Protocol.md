## Longitud de comandos
- Username = 10 caracteres
- Mensaje = 25 caracteres
- Informacion de usuario buscado = 50 caracteres

## Ingreso de usuario
- Comando: L<Username>
- Respuesta: 0 Existoso, -1 Fallido

## Registro de usuario
- Comando: R<Username>
- Respuesta: 1 Exitoso, 0 Fallido

## Iniciar envio de mensaje a usuario
- Comando: E<Username>
- Respuesta: 0 Exitoso (usuario encontrado), -1 Fallido (No encontro usuario)

## Ver mensajes
- Comando: V
- Respuesta: 

## Enviar archivo
- Comando: F<Directorio><Username>
- Respuesta: 

## Cerrar sesion/Salir
- Comando: X
- Respuesta: 1 Exitoso

sendfile
## Recibir archivo


## Nueva sala
- Comando: S
- Respuesta: Id de sala

## Unirse a sala
- Comando: U <Id sala>
- Respuesta: 1 Exitoso,  -1 Fallido

## Eliminar sala ()
## Cantidad de usuarios conectados
- Comando: O

## Buscar usuario
- Comando: B <Username>
- Respuesta: Estado actual de usuario o no encontrado