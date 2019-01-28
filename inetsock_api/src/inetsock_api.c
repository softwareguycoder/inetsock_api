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
// SOCKET struct - wraps a socket file descriptor in an opaque type that also
// carries information about the state and type of socket.

struct _tagSOCKET {
	int				nSocketDescriptor;		/* Linux file descriptor */
	long			dwLastError;	/* WSA* error code for the last network error */
	SOCKET_TYPE 	sockType;		/* What type of socket is this? */
	SOCKET_STATE 	sockState;		/* What state is this socket in? */
};

///////////////////////////////////////////////////////////////////////////////
// CloseSocket - Closes the socket with the specified handle and releases the
// resources used by the socket back to the operating system.  Does nothing if
// the socket has already been closed or its resources have already been released.
// All active network connections on this socket will be terminated.

void CloseSocket(HSOCKET hSocket)
{
	// TODO: Add implementation code here
}

///////////////////////////////////////////////////////////////////////////////
// ConnectToServer - sets up a client socket and then connects to a server at a
// specified host name and port.  Provides the caller an ability to set up a
// callback function referred to by a given function pointer as what is called
// when the socket's state changes
//

void ConnectToServer(HSOCKET hSocket, const char* pszHostAddress, int nPort,
		LPSOCKET_EVENT_CALLBACK lpfnClientCallback)
{
	// TODO: Add implementation code here
}

///////////////////////////////////////////////////////////////////////////////
// GetLastError - Provides the operating system error code corresponding to the
// error condition last experienced by the socket.  Returns zero if there is
// currently no error or if the socket's state is not SOCKET_STATE_ERROR.

long GetLastError(HSOCKET hSocket)
{
	// TODO: Add implementation code here
	return 0L;
}


///////////////////////////////////////////////////////////////////////////////
// GetSocketState - gets the current state of the socket whose handle is
// specified in the hSocket parameter.

SOCKET_STATE GetSocketState(HSOCKET hSocket)
{
	// TODO: Add implementation code here
	return SOCKET_STATE_UNKNOWN;
}

///////////////////////////////////////////////////////////////////////////////
// GetSocketType - gets the type of socket created

SOCKET_TYPE GetSocketType(HSOCKET hSocket)
{
	// TODO: Add implementation code here
	return SOCKET_TYPE_UNKNOWN;
}

///////////////////////////////////////////////////////////////////////////////
// OpenSocket - creates a new socket in the operating system of the specified
// type, and, depending on the type of socket, prepares it according to whether
// the socket is for a client, data, or server connection.  Returns a handle
// to the new socket.

HSOCKET OpenSocket(SOCKET_TYPE type)
{
	// TODO: Add implementation code here
	return (HSOCKET)INVALID_HANDLE_VALUE;
}

///////////////////////////////////////////////////////////////////////////////
// RunServer - Runs a server on the specified port, using the specified server
// socket for listening for incoming connections.  A callback provided by the
// caller is called for each change in the server socket's state.  Run the
// listen/accept/respond loop for a server socket and fires the calllback as
// needed.  The socket handle passed should already have been opened by a call
// to OpenSocket.

void RunServer(HSOCKET hSocket, int nPort,
	LPSOCKET_EVENT_CALLBACK lpfnServerCallback,
	LPSOCKET_EVENT_CALLBACK lpfnCommCallback)
{
	// TODO: Add implementation code here
}

///////////////////////////////////////////////////////////////////////////////
// SetSocketState - Sets the state of the specified socket to the specified
// SOCKET_STATE value.  If the socket handle has an invalid value, this function does
// nothing.

void SetSocketState(HSOCKET hSocket, SOCKET_STATE newState)
{
	if (INVALID_HANDLE_VALUE == hSocket)
		return;

	if (hSocket->nSocketDescriptor <= INVALID_SOCKET_DESCRIPTOR)
		return;

	if (SOCKET_STATE_ERROR == hSocket->sockState)
		return;

	hSocket->sockState = newState;
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
}

///////////////////////////////////////////////////////////////////////////////
