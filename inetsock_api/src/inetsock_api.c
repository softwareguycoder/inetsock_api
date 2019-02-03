// inetsock_api.c - provides implementations for opaque types, methods, and
// callbacks which together abstract away the details of calling the,
// e.g., SocketDemoUtils_* functions
//

#include "stdafx.h"

#include "inetsock_api.h"

/**
 * @brief Defines the value for an invalid Socket Descriptor. In Linux,
 * all socket descritors are positive integers greater than 2.  So, 2
 * (mapped to STDERR) and less represents an invalid descriptor.  Testing
 * for this is how we know that an error condition exists.
 */
#ifndef INVALID_SOCKET_DESCRIPTOR
#define INVALID_SOCKET_DESCRIPTOR 		2
#endif //INVALID_SOCKET_DESCRIPTOR

///////////////////////////////////////////////////////////////////////////////
// free_buffer - Internal function that is not exposed to users of this library.
// This function is in charge, simply, of calling free() on a non-NULL void
// pointer.

void free_buffer(void **ppBuffer)
{
    if (ppBuffer == NULL || *ppBuffer == NULL)
        return;     // Nothing to do since there is no address referenced

    free(*ppBuffer);
    *ppBuffer = NULL;
}

///////////////////////////////////////////////////////////////////////////////
// SOCKET struct - wraps a socket file descriptor in an opaque type that also
// carries information about the state and type of socket.

struct _tagSOCKET {
	int				nSocketDescriptor;		/* Linux file descriptor */
	long			dwLastError;	/* WSA* error code for the last network error */
	SOCKET_TYPE 	sockType;		/* What type of socket is this? */
	SOCKET_STATE 	sockState;		/* What state is this socket in? */
	LPSOCKET_EVENT_ROUTINE lpfnCallback;	/* Function that gets called when something happens on this socket */
};

///////////////////////////////////////////////////////////////////////////////
// CloseSocket - Closes the socket with the specified handle and releases the
// resources used by the socket back to the operating system.  Does nothing if
// the socket has already been closed or its resources have already been released.
// All active network connections on this socket will be terminated.

void CloseSocket(HSOCKET hSocket)
{
	if (INVALID_HANDLE_VALUE == hSocket)
		return;

	SetSocketState(hSocket, SOCKET_STATE_CLOSING);

	// This library sits on top of the inetsock_core library; so, call that library's
	// SocketDemoUtils_close function to actually do the closing.
	SocketDemoUtils_close(hSocket->nSocketDescriptor);

	SetSocketState(hSocket, SOCKET_STATE_CLOSED);

	// This library manages calling malloc() and free() on socket handles for
	// the caller.  Sincer we have closed this socket, time to free it.
	free_buffer((void**)&hSocket);
}

///////////////////////////////////////////////////////////////////////////////
// ConnectToServer - sets up a client socket and then connects to a server at a
// specified host name and port.  Provides the caller an ability to set up a
// callback function referred to by a given function pointer as what is called
// when the socket's state changes
//

void ConnectToServer(HSOCKET hSocket, const char* pszHostAddress, int nPort)
{
	// TODO: Add implementation code here
}

///////////////////////////////////////////////////////////////////////////////
// GetLastError - Provides the operating system error code corresponding to the
// error condition last experienced by the socket.  Returns zero if there is
// currently no error or if the socket's state is not SOCKET_STATE_ERROR.

long GetLastError(HSOCKET hSocket)
{
	if (SOCKET_STATE_ERROR != GetSocketState(hSocket))
		return 0;		/* no error */

	/* return the errno value, since the socket being in an error state
	 * most likely means that this function is being called from the callback
	 * the user of this library set up right after the callback was fired
	 * when a inetsock_core library function failed.
	 */

	return errno;
}


///////////////////////////////////////////////////////////////////////////////
// GetSocketState - gets the current state of the socket whose handle is
// specified in the hSocket parameter.

SOCKET_STATE GetSocketState(HSOCKET hSocket)
{
	SOCKET_STATE result = SOCKET_STATE_UNKNOWN;	// default return value

	// Obviously, an invalid socket handle does not have a state
	if (INVALID_HANDLE_VALUE == hSocket)
		return result;

	result = hSocket->sockState;

	return result;
}

///////////////////////////////////////////////////////////////////////////////
// GetSocketType - gets the type of socket created

SOCKET_TYPE GetSocketType(HSOCKET hSocket)
{
	SOCKET_TYPE result = SOCKET_TYPE_UNKNOWN;

	if (INVALID_HANDLE_VALUE == hSocket)
		return result;

	result = hSocket->sockType;

	return result;
}

///////////////////////////////////////////////////////////////////////////////
// OpenSocket - creates a new socket in the operating system of the specified
// type, and, depending on the type of socket, prepares it according to whether
// the socket is for a client, data, or server connection.  Returns a handle
// to the new socket.

HSOCKET OpenSocket(SOCKET_TYPE type, LPSOCKET_EVENT_ROUTINE lpfnCallback)
{
	HSOCKET hSocket = (HSOCKET)INVALID_HANDLE_VALUE;

	if (type == SOCKET_TYPE_UNKNOWN)
		return hSocket;

	if (lpfnCallback == NULL)
		return hSocket;

	// Use malloc to dynamically allocate a new HSOCKET (really a new
	// tagSOCKET structure but HSOCKET is a pointer type).
	hSocket = (HSOCKET)malloc(sizeof(HSOCKET));

	// Initialize the new socket with a call to SocketDemoUtils_createTcpSocket
	hSocket->nSocketDescriptor = SocketDemoUtils_createTcpSocket();
	if (hSocket->nSocketDescriptor <= 0)
	{
		// Failed to open new socket.  Call GetLastError to get the errno
		// and then use strerror to tell the user.
		SetSocketState(hSocket, SOCKET_STATE_ERROR);

		free_buffer((void**)&hSocket);

		// Call the callback -- this is the only time that we call the callback
		// prior to it being associated with a socket handle
		lpfnCallback(INVALID_HANDLE_VALUE, NULL);

		return INVALID_HANDLE_VALUE;
	}

	// Associate the callback with the new socket
	hSocket->lpfnCallback = lpfnCallback;

	SetSocketState(hSocket, SOCKET_STATE_OPENED);

	return hSocket;
}

///////////////////////////////////////////////////////////////////////////////
// RunServer - Runs a server on the specified port, using the specified server
// socket for listening for incoming connections.  A callback provided by the
// caller is called for each change in the server socket's state.  Run the
// listen/accept/respond loop for a server socket and fires the calllback as
// needed.  The socket handle passed should already have been opened by a call
// to OpenSocket.

void RunServer(HSOCKET hSocket, int nPort)
{
	// TODO: Add implementation code here
}

///////////////////////////////////////////////////////////////////////////////
// SetSocketState - Sets the state of the specified socket to the specified
// SOCKET_STATE value.  If the socket handle has an invalid value, this function
// does nothing.  When this function has changed the state of the socket to the
// new value, then the callback that the user of this library has registered
// is called.  The user of this library can then get the state of the socket
// by calling GetSocketState on the socket handle.

void SetSocketState(HSOCKET hSocket, SOCKET_STATE newState)
{
	if (INVALID_HANDLE_VALUE == hSocket)
		return;

	if (hSocket->nSocketDescriptor <= INVALID_SOCKET_DESCRIPTOR)
		return;

	if (SOCKET_STATE_ERROR == hSocket->sockState)
		return;

	hSocket->sockState = newState;

	// Fire the callback to let the user of this socket know that something
	// neat happened.
	if (NULL == hSocket->lpfnCallback)
		return;

	hSocket->lpfnCallback(hSocket, NULL);
}

///////////////////////////////////////////////////////////////////////////////
// SetSocketType - Sets the type of the socket specified by the handle provided
// to the specified type (one of the SOCKET_TYPE values: CLIENT, DATA, or SERVER).
// If the socket is in the SOCKET_STATE_ERROR state or if the handle is
// INVALID_HANDLE_VALUE, this function does nothing.  This function will also
// only operate if the type has not already been set to something other than
// SOCKET_TYPE_UNKNOWN, which is the default.  Once the socket's type has been
// set, it should not be altered in the same program for the same socket.

void SetSocketType(HSOCKET hSocket, SOCKET_TYPE newType)
{
	if (INVALID_HANDLE_VALUE == hSocket)
		return;

	if (hSocket->nSocketDescriptor <= INVALID_SOCKET_DESCRIPTOR)
		return;

	if (SOCKET_STATE_ERROR == hSocket->sockState)
		return;

	if (SOCKET_TYPE_UNKNOWN != hSocket->sockType)
		return;

	hSocket->sockType = newType;

	// Fire the callback to let the user of this socket know that something
	// neat happened.
	if (NULL == hSocket->lpfnCallback)
		return;

	hSocket->lpfnCallback(hSocket, NULL);
}

///////////////////////////////////////////////////////////////////////////////

