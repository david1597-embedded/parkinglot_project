using OpenCvSharp;
using System;
using System.Data;
using System.Collections.Concurrent;
using System.IO;
using System.Net;
using System.Net.WebSockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

//namespace ConsoleApp1
/*{
    internal class Program
    {
        static void Main(string[] args)
        {
            Console.WriteLine("Hello, World!");

            using var capture = new VideoCapture(0);

            if(!capture.IsOpened())
            {
                Console.WriteLine("Cannot open Camera. Attempting to open Index 1 ...");
                capture.Open(1);
                if(!capture.IsOpened())
                {
                    Console.WriteLine("Cannot open Camera. Program Exit!");
                    return;
                }                   
            }

            //Camera Setting
            capture.Set(VideoCaptureProperties.FrameWidth, 640); //Width
            capture.Set(VideoCaptureProperties.FrameHeight, 480);//Height

            try
            {
                capture.Set(VideoCaptureProperties.FourCC, FourCC.MJPG);
                Console.WriteLine("MJPEG FORMOAT SETTING SUCCESSED");
            }
            catch
            {
                Console.WriteLine("MJPEG SETTING FAILED. USING BASIC FORMAT");
            }

            //Window Instantiating
            string windowName = "CCTV 1";
            Cv2.NamedWindow(windowName, WindowFlags.AutoSize);

            //Dealing with Frame Loop
            using var frame = new Mat();
            Console.WriteLine("Camera recording started. Exit with ESC");

            while(true)
            {
                if(!capture.Read(frame) || frame.Empty())
                {
                    Console.WriteLine("Failed to read frame , Retrying...");
                    System.Threading.Thread.Sleep(100);
                    continue;
                }

                //Adding text
                string timestamp = DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss");
                Cv2.PutText(frame, $"CCTV 1 - {timestamp}", new Point(10, 30),
                    HersheyFonts.HersheySimplex, 1.0 ,Scalar.Green, 2);
                Cv2.PutText(frame, windowName, new Point(10, 60),
                    HersheyFonts.HersheySimplex, 1.0, Scalar.Green, 2);


                //Framing Window
                Cv2.ImShow(windowName, frame);

                if(Cv2.WaitKey(33) == 27)
                {
                    break;
                }
            }

            //Realising Resources
            Cv2.DestroyWindow(windowName);
            capture.Release();
            Console.WriteLine("Camera program Exit.");
        }
    }
}*/


class Program
{
    private static readonly HttpListener _listner = new HttpListener();
    private static VideoCapture _capture =new VideoCapture();
    private static bool _isRunning;
    private static readonly ConcurrentDictionary<string, WebSocket> _clients = new ConcurrentDictionary<string, WebSocket>();

    static async Task Main(string[] args)
    {
        //Initializing Camera
        _capture = new VideoCapture(0);
        if(!_capture.IsOpened())
        {
            Console.WriteLine("Failed to open camera. Attempting to open Index 1");
            _capture.Open(1);
            if(!_capture.IsOpened())
            {
                Console.WriteLine("Failed to connect camera. Program Exit");
                return;
            }         
        }

        //Camera Window Setting
        //Width
        _capture.Set(VideoCaptureProperties.FrameWidth, 640);
        //Height
        _capture.Set(VideoCaptureProperties.FrameHeight, 640);

        try
        {
            _capture.Set(VideoCaptureProperties.FourCC, FourCC.MJPG);
            Console.WriteLine("Sucessed to set MJPEG FORMAT");
        }
        catch
        {
            Console.WriteLine("Failed to set MJPEG FORMAT. Using Basic format");
        }

        //WebSock Server Implementation
        _listner.Prefixes.Add("http://*:8080/");
        try
        {
            _listner.Start();
            Console.WriteLine("Start Server : http://localhost:8080");
        }
        catch (Exception ex) 
        { 
            Console.WriteLine($"Failed to start server : {ex.Message}");
            return;
        }

        //Startring to stream camera
        _isRunning = true;
        Task streamTask = Task.Run(() => StreamCameraAsync());

        //Dealing with HTTP/WebScocket 
        while(true)
        {
            try
            {
                var context = await _listner.GetContextAsync();
                if(context.Request.IsWebSocketRequest)
                {
                    var wsContext = await context.AcceptWebSocketAsync(null);
                    string clientId = Guid.NewGuid().ToString();
                    _clients.TryAdd(clientId, wsContext.WebSocket);
                    _ = HandleWebSocketAsync(wsContext.WebSocket, clientId);
                }
                else if((context.Request.Url.LocalPath == "/" )|| (context.Request.Url.LocalPath == "/index.html"))
                {
                    string htmlPath = Path.Combine(Directory.GetCurrentDirectory(), "www", "index.html");
                    if (File.Exists(htmlPath))
                    {
                        byte[] htmlBytes = File.ReadAllBytes(htmlPath);
                        context.Response.ContentType = "text/html";
                        await context.Response.OutputStream.WriteAsync(htmlBytes, 0, htmlBytes.Length);
                    }
                    else
                    {
                        context.Response.StatusCode = 404;
                        Console.WriteLine($"HTML FILE LOSS: {htmlPath}");
                    }
                    context.Response.Close();
                }
                else
                {
                    context.Response.StatusCode = 404;
                    context.Response.Close();
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error Request Task Processing : {ex.Message}" );
            }
        }
    }

    static async Task StreamCameraAsync()
    {
        using var frame = new Mat();
        while(_isRunning)
        {
            try
            {
                if(!(_capture.Read(frame)) || frame.Empty())
                {
                    Console.WriteLine("Failed to read frame, Retrying...");
                    await Task.Delay(100);
                    continue;
                }

                //Adding text
                string timestamp = DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss");
                Cv2.PutText(frame, $"CCTV 1 - {timestamp}", new Point(10, 30),
                    HersheyFonts.HersheySimplex, 1.0, Scalar.Green, 2);
                Cv2.PutText(frame, "CCTV 1", new Point(10, 60),
                    HersheyFonts.HersheySimplex, 1.0, Scalar.Green, 2);

                //Converting to JPEG
                byte[] jpegData = frame.ToBytes(".jpg", new int[] { (int)ImwriteFlags.JpegQuality, 70 });
                string base64String = Convert.ToBase64String(jpegData);
                string message = $"0|data:image/jpeg;base64,{base64String}";

                //Transmitting to Client
                foreach(var client in _clients)
                {
                    if(client.Value.State == WebSocketState.Open)
                    {
                        try
                        {
                            byte[] buffer = Encoding.UTF8.GetBytes(message);
                            await client.Value.SendAsync(new ArraySegment<byte>(buffer), WebSocketMessageType.Text, true, CancellationToken.None);
                        }
                        catch
                        {
                            _clients.TryRemove(client.Key, out _);
                        }
                    }
                }
                await Task.Delay(33);
            }
            catch (Exception ex) 
            {
                Console.WriteLine($"Camera Error : {ex.Message}");
                await Task.Delay(1000);
            }
        }
    }

    static async Task HandleWebSocketAsync(WebSocket ws, string clientId)
    {
        try
        {
            while (ws.State == WebSocketState.Open)
            {
                var buffer = new byte[1024];
                var result = await ws.ReceiveAsync(new ArraySegment<byte>(buffer), CancellationToken.None);
                if (result.MessageType == WebSocketMessageType.Close)
                    break;
            }
        }
        catch (Exception ex)
        {
            Console.WriteLine($"WebSocket Erorr: {ex.Message}");
        }
        finally
        {
            _clients.TryRemove(clientId, out _);
            await ws.CloseAsync(WebSocketCloseStatus.NormalClosure, "Connection End", CancellationToken.None);
        }
    }
}