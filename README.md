# net_framework
-------------------
## [1 Introduction](#Introduction)
## [2 Interface](#Interface)
### [2.1 AP](#AP)
#### [2.1.1 TcpHandleInit](#TcpHandleInit)
#### [2.1.2 TcpHandlerConnected](#TcpHandlerConnected)
#### [2.1.3 TcpHandlerDisconnected](#TcpHandlerDisconnected)
#### [2.1.4 FinishTcpHandle](#FinishTcpHandle)
#### [2.1.5 MsgHandleUnpack](#MsgHandleUnpack)
#### [2.1.6 MsgHandleRoute](#MsgHandleRoute)
### [2.2 JOB](#JOB)
#### [2.2.1 MsgHandleInit](#MsgHandleInit)
#### [2.2.2 MsgHandleProcessing](#MsgHandleProcessing)
#### [2.2.3 MsgHandleFinish](#MsgHandleFinish)
## [3 Dependence](#dependence)
## [4 Compile](#compile)
--------------------
## 1 Introduction <a name="Introduction"/>
*       This framework processing detail of net socket.With it, you can focus on business 
		logic and do not care of the problem of socket and concurrence.You just need to 
		define some interface to deal with the business logic and add it to plugins, than 
		run the framework.

## 2 Interface <a name="Interface"/>

### 2.1 AP (TCP) <a name="AP"/>

#### 2.1.1 TcpHandleInit <a name="TcpHandleInit"/>
*       Define: INT32 TcpHandleInit(const CHAR * file);
*       This interface initial some resources for tcp handler.The parameters of file 
		support the path of configure file.

#### 2.1.2 TcpHandlerConnected <a name="TcpHandlerConnected"/>
*       Define: INT32 TcpHandlerConnected(INT32 sock_fd, const CHAR * ip, INT32 port);
*       This interface involved by framework when socket connection created.You can do 
		some logic check for connection.
        For example: You can verify the connection, if connection is invalid return 
        not zero, than framework will disconnect.

#### 2.1.3 TcpHandlerDisconnected <a name="TcpHandlerDisconnected"/>
*       Define: INT32 TcpHandlerDisconnected(INT32 sock_fd, const CHAR * ip, INT32 port);
*       This interface involved by framework when socket disconnected,You can free 
		resources that allocated by TcpHandlerConnected.

#### 2.1.4 FinishTcpHandle <a name="FinishTcpHandle"/>
*       Define: INT32 FinishTcpHandle();
*       This interface involved by framework when AP shutdown graceful.You can free 
		resource that allocated by TcpHandleInit.

#### 2.1.5 MsgHandleUnpack <a name="MsgHandleUnpack"/>
*       Define: INT32 MsgHandleUnpack(const CHAR * data, INT32 size);
*       This interface help you to split package into multiple datagrams.The return 
		value indicates the length of one datagram.

#### 2.1.6 MsgHandleRoute <a name="MsgHandleRoute"/>
*       Define: INT32 MsgHandleRoute(const CHAR * data, INT32 size);
*       This interface make the Load banlance of JOB possible,The return value 
		indicates the ID of JOB.

### 2.2 JOB <a name="JOB"/>
#### 2.2.1 MsgHandleInit <a name="MsgHandleInit"/>
*       Define: INT32 MsgHandleInit(const CHAR * file);
*       This interface involved by framework when job initial.You can allocate 
		resource here.

#### 2.2.2 MsgHandleProcessing <a name="MsgHandleProcessing"/>
*       Define: INT32 MsgHandleProcessing(const CHAR * data, INT32 size, CHAR * out, 
											INT32 * out_size,
                                            AsyncCallBackWrite func, VOID * params, 
                                            INT32 params_size);
                                                
*       This interface is the main entry of business logic.You can implement it 
		synchronous and asynchronous.The value returned zero indicates success,
		non zero failed.

#### 2.2.3 MsgHandleFinish <a name="MsgHandleFinish"/>
*       Define: INT32 MsgHandleFinish();
*       This interface involved by framework when JOB shutdown graceful.You can free 
		resource allocated by MsgHandleInit.


## 3 Dependence <a name="dependence"/>
### libcurl: https://curl.haxx.se/

*       wget https://curl.haxx.se/download/curl-7.56.1.tar.gz 
        tar zxvf curl-7.56.1.tar.gz
        cd curl-7.56.1 
        ./configure
        make
        sudo make install
        
### commlib: https://github.com/changyuezhou/commlib.git

## 4 Compile <a name="compile"/>
*       git clone --recursive https://github.com/changyuezhou/net_framework.git
        mkdir build
        cd build
        cmake ../net_framework
        make
        
##         
             
processing epoll model
