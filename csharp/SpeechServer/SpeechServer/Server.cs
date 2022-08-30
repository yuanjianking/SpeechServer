using System.Diagnostics.CodeAnalysis;
using System.Net;
using System.Net.Sockets;
using System.Text;

namespace SpeechServer
{
    internal class Server
    {
        private const string host = "127.0.0.1";
        private const int BufferSize = 1024;
        private const int port = 8885;
        private const int request = 10;

        private readonly byte[] result = new byte[BufferSize];
        private readonly int myProt = port;   //端口
        private readonly Socket serverSocket = new (AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

        private static bool IsNotNull([NotNullWhen(true)] object? obj) => obj != null;
        public void Start()
        {
            //服务器IP地址
            IPAddress ip = IPAddress.Parse(host);           
            serverSocket.Bind(new IPEndPoint(ip, myProt));  //绑定IP地址：端口
            serverSocket.Listen(request);    //设定最多10个排队连接请求
            Console.WriteLine("启动监听{0}成功", serverSocket.LocalEndPoint?.ToString());
            //通过Clientsoket发送数据
            Thread myThread = new (ListenClientConnect);
            myThread.Start();
            Console.ReadLine();
        }

        /// <summary>
        /// 监听客户端连接
        /// </summary>
        private void ListenClientConnect()
        {
            while (true)
            {
                Socket clientSocket = serverSocket.Accept();
                clientSocket.Send(Encoding.ASCII.GetBytes("Server Say Hello"));
                Thread receiveThread = new (ReceiveMessage);
                receiveThread.Start(clientSocket);
            }
        }

        /// <summary>
        /// 接收消息
        /// </summary>
        /// <param name="clientSocket"></param>
        private void ReceiveMessage(object? clientSocket)
        {
            if (IsNotNull(clientSocket))
            { 
                Socket myClientSocket = (Socket)clientSocket;
                while (true)
                {
                    try
                    {
                        //通过clientSocket接收数据
                        int receiveNumber = myClientSocket.Receive(result);
                        Console.WriteLine("接收客户端{0}消息{1}", myClientSocket.RemoteEndPoint?.ToString(), Encoding.ASCII.GetString(result, 0, receiveNumber));
                    }
                    catch (Exception ex)
                    {
                        Console.WriteLine(ex.Message);
                        myClientSocket.Shutdown(SocketShutdown.Both);
                        myClientSocket.Close();
                        break;
                    }
                }
            }
        }

    }
}
