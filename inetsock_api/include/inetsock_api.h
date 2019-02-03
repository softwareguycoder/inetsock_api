// inetsock_api.h - file that defines prototypes for functions, typedefs, and data structures that
// together form an API that abstracts away the details of setting up basic clients and servers
//

#ifndef __INETSOCK_API_H__
#define __INETSOCK_API_H__

#ifndef INVALID_HANDLE_VALUE
#define INVALID_HANDLE_VALUE	NULL
#endif

/** @brief Values that indicate the state of a socket connection, i.e., a HSOCKET handle. */
typedef enum 
{
	SOCKET_STATE_ACCEPTED,
	SOCKET_STATE_ACCEPTING,
	SOCKET_STATE_BINDING,
	SOCKET_STATE_BOUND,
	SOCKET_STATE_CLOSING,
	SOCKET_STATE_CLOSED,
	SOCKET_STATE_CONNECTING,
	SOCKET_STATE_CONNECTED,
	SOCKET_STATE_DISCONNECTED,
	SOCKET_STATE_ERROR,
	SOCKET_STATE_OPENED,
	SOCKET_STATE_SENDING,
	SOCKET_STATE_SENT,
	SOCKET_STATE_READY,	/* socket is ready for use! */
	SOCKET_STATE_RECEIVING,
	SOCKET_STATE_RECEIVED,
	SOCKET_STATE_LISTENING,
	SOCKET_STATE_UNKNOWN,
} SOCKET_STATE;

/** @brief Values that indicate what type of socket we are using (e.g., a client socket or a
 *  server socket)
*/
typedef enum 
{
	SOCKET_TYPE_CLIENT,
	SOCKET_TYPE_DATA,		/** typically only ever used in a FTP connection */
	SOCKET_TYPE_SERVER,
	SOCKET_TYPE_UNKNOWN
} SOCKET_TYPE;

/**
 * \brief Opaque type that represents a socket handle.  A socket handle is a Windows-ish concept
 * whereas Linux has a int that is the socket file descriptor.  However, this allows functions to
 * use sockets without worrying about how they are implemented.
 */
typedef struct _tagSOCKET *HSOCKET;

/*typedef struct _tagSOCKETSENDDATA {
	HSOCKET hSocket;
	char* pszData;
	int nBytesSent;
} SOCKETSENDDATA, *PSOCKETSENDDATA;*/

/**
 * @brief Pointer to a function that will be executed as a callback each time a given socket's
 * state changes.
 * @param hSocket Handle to the socket that is firing events through this particular callback.
 * @param lpUserState Points to additional user state information.  Can be of any type (in
 * principle).
 * @remarks !!IMPORTANT!! All implementers of this callback must set the socket state to SOCKET_STATE_READY
 * prior to returning.
 */
typedef void (*LPSOCKET_EVENT_ROUTINE)(HSOCKET hSocket, void* lpUserState);

/**
 * @brief Closes all connections on the specified socket and releases its resources back to the
 * operating system.
 * @param HSOCKET handle of the socket to be closed.
 * @remarks This function does nothing if the socket is already in the SOCKET_STATE_CLOSED state or
 * has had its resources released.
 */
void CloseSocket(HSOCKET hSocket);

/**
 * @brief Connects to a remote server using the speified socket, host address, and port number.
 * @param hSocket Handle to the socket that should be used for connection.  Must have already been
 * opened with OpenClientSocket.
 * @param pszHostAddress Points to a buffer that contains a null terminated string that contains either
 * a valid IPv4 address or DNS-resolvable hostname.
 * @param nPort Integer specifying the port on which the server is listening.
 */
void ConnectToServer(HSOCKET hSocket, const char* pszHostAddress, int nPort);

/**
 * @brief Call this function if the state of the specified socket is SOCKET_STATE_ERROR.
 * @param hSocket Socket handle (of type HSOCKET) that references the socket you want information for.
 * @return Long integer that corresponds to the operating system errno code.
 */
long GetLastError(HSOCKET hSocket);

/**
 * @brief Gets the SOCKET_STATE value that corresponds to the state that the socket with
 * the specified handle is currently in.
 * @param hSocket Socket handle (of type HSOCKET) that references the socket you want information for.
 * @returns One of the SOCKET_STATE values that corresponds to the socket's state.
 */
SOCKET_STATE GetSocketState(HSOCKET hSocket);

/**
 * @brief Gets the SOCKET_TYPE value that the socket with the specified handle was opened as.
 * @param hSocket Socket handle (of type HSOCKET) that references the socket you want information for.
 * @returns One of the SOCKET_TYPE values.
 */
SOCKET_TYPE GetSocketType(HSOCKET hSocket);

/**
 * @brief Forcibly terminates the program with the ERROR exit code while also closing
 * and deallocating the memory for the socket.
 * @param hSocket Socket handle to the socket to be killed.
 * @param pszMessage Message to display to the user.
 */
void KillSocket(HSOCKET hSocket, const char* pszMessage);
/**
 * @brief Opens (creates) a new socket of the specified type.
 * @param type One of the SOCKET_TYPE values.  Specifies which type of socket is to be opened.
 */
HSOCKET OpenSocket(SOCKET_TYPE type, LPSOCKET_EVENT_ROUTINE lpfnCallback);

/**
 * @brief Runs the listen/accept/connect loop for a server on a specified port number, using the
 * socket specified as the server's TCP endpoint.  The server runs on the same local machine
 * as which the calling program resides.
 * @param hSocket Handle to the server TCP endpoint's socket.
 * @param nPort Port number on which the server is to listen for incoming connections.
 */
void RunServer(HSOCKET hSocket, int nPort);

/**
 * @brief Sends data over an open and connected socket.  This function will check the socket
 * for a SOCKET_STATE_CONNECTED state, and refuses to run if this is not the case.
 * @param hSocket Socket handle representing the TCP endpoint over which to send data.
 * @param pszData Pointer to a buffer containing the data to be sent.
 * @returns ERROR if the operation failed; number of bytes sent otherwise.
 *	If the ERROR value is returned, errno should be examined to determine the
 *  cause of the error.  The socket's state will be set to SOCKET_STATE_ERROR.
 */
int Send(HSOCKET hSocket, const char* pszData);

/**
 * @brief Sets the state of the specified socket to a new value as indicated by the newState
 * parameter.
 * @param hSocket Handle to the socket for which the state is to be changed.
 * @param newState One of the SOCKET_STATE values that defines the new state.
 * @remarks If this socket has a callback registered, then the callback is called with a
 * NULL value for the lpUserState parameter.  This function is an alias for SetSocketStateEx,
 * here we do not pass in any user state.
 */
void SetSocketState(HSOCKET hSocket, SOCKET_STATE newState);

/**
 * @brief Sets the state of the specified socket to a new value as indicated by the newState
 * parameter.
 * @param hSocket Handle to the socket for which the state is to be changed.
 * @param newState One of the SOCKET_STATE values that defines the new state.
 * @param lpUserState State bag to be passed along to the socket callback.
 * @remarks If this socket has a callback registered, then the callback is called with the
 * value passed in for the lpUserState parameter. *
 */
void SetSocketStateEx(HSOCKET hSocket, SOCKET_STATE newState, void* lpUserState);

/**
 * @brief Sets the state of the specified socket to a new value as indicated by the newState
 * parameter.
 * @param hSocket Handle to the socket for which the state is to be changed.
 * @param newType One of the SOCKET_TYPE values that defines the new type.
 * @remarks If this socket has a callback registered, then the callback is called with a
 * NULL value for the lpUserState parameter.
 */
void SetSocketType(HSOCKET hSocket, SOCKET_TYPE newType);


#endif /* __INETSOCK_API_H__ */
