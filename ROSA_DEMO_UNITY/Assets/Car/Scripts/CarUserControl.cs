using System;
using System.IO.Ports;
using System.Threading;
using UnityEngine;


namespace UnityStandardAssets.Vehicles.Car
{
    [RequireComponent(typeof (CarController))]
    public class CarUserControl : MonoBehaviour
    {
        private CarController m_Car; // the car controller we want to use

        public Light worldLight;
        public Light headlightLeft;
        public Light headlightRight;

        Thread serialThread;
        bool _readSerial = true;
        SerialReader sr;

        private string[] _SerialPorts;
        private string _SelectedSerialPort = "COM3";
        private int _PotValue = 0;
        private bool _Headlights = false;
        private bool _Brakes = false;
        private bool _BrakeActive = false;
        private string _Steering = "Neutral";

        private void Awake()
        {
            // get the car controller
            m_Car = GetComponent<CarController>();

            _SerialPorts = SerialPort.GetPortNames();
            if(_SerialPorts.Length > 0)
            {
                _SelectedSerialPort = _SerialPorts[0];
            }

            sr = new SerialReader(_SelectedSerialPort);

            serialThread = new Thread(ReadSerialPort);
            serialThread.Start();
        }


        private void FixedUpdate()
        {
            // pass the input to the car!
            float h = 0; // CrossPlatformInputManager.GetAxis("Horizontal");
            float v = 0; // CrossPlatformInputManager.GetAxis("Vertical");

            v = (float)_PotValue / 1000.0f;

            if (_Steering == "Left")
                h = -1;
            else if (_Steering == "Right")
                h = 1;

            if(_Brakes)
            {
                v = -1;
            }

            if(headlightLeft != null && headlightRight != null & _Headlights)
            {
                headlightRight.intensity = 5;
                headlightLeft.intensity = 5;
            }
            else
            {
                headlightRight.intensity = 0;
                headlightLeft.intensity = 0;
            }

            if (Input.GetKeyDown("space") && worldLight != null)
            {
                if (worldLight.intensity == 0)
                    worldLight.intensity = 0.8f;
                else
                {
                    worldLight.intensity = 0;
                }
            }


#if !MOBILE_INPUT
            float handbrake = 0; // CrossPlatformInputManager.GetAxis("Jump");
            m_Car.Move(h, v, v, handbrake);
#else
            m_Car.Move(h, v, v, 0f);
#endif
        }

        void OnApplicationQuit()
        {
            _readSerial = false;
        }

        private void ReadSerialPort()
        {
            int pot = 0;
            int headlights = 0;
            int brakes = 0;
            int brake_active = 0;
            int steeringLeft = 0;
            int steeringRight = 0;

            while (_readSerial)
            {
                // Current format of data { potentiometer val, headlights, brake button down, brake active(ABS) }
                string[] values = sr.ReadSerial('|').Split(',');

                try
                {
                    int.TryParse(values[0], out pot);
                    int.TryParse(values[1], out headlights);
                    int.TryParse(values[2], out brakes);
                    int.TryParse(values[3], out brake_active);
                    int.TryParse(values[4], out steeringLeft);
                    int.TryParse(values[5], out steeringRight);

                    _PotValue = pot;
                    _Headlights = headlights != 0;
                    _Brakes = brakes != 0;
                    _BrakeActive = brake_active != 0;

                    if (steeringLeft == 1)
                        _Steering = "Left";
                    else if (steeringRight == 1)
                        _Steering = "Right";
                    else _Steering = "Neutral";
                }
                catch (Exception e)
                {
                    Debug.Log(e.Message);
                }
            }
            Debug.Log("Serial thread is DYING!");
        }
    }

    class SerialReader
    {
        SerialPort _serialPort = new SerialPort();

        // Default property values
        int _baudRate = 57600;
        int _dataBits = 8;
        int _readTimeout = 5;
        int _writeTimeout = 5;
        Handshake _handshake = Handshake.None;
        Parity _parity = Parity.None;
        //string _portName = "COM3";
        StopBits _stopBits = StopBits.One;

        public SerialReader(string portName)
        {
            // Create a new SerialPort object with default settings.
            _serialPort = new SerialPort();

            // Allow the user to set the appropriate properties.
            _serialPort.PortName = portName;
            _serialPort.BaudRate = _baudRate;
            _serialPort.Parity = _parity;
            _serialPort.DataBits = _dataBits;
            _serialPort.StopBits = _stopBits;
            _serialPort.Handshake = _handshake;

            // Set the read/write timeouts
            _serialPort.ReadTimeout = _readTimeout;
            _serialPort.WriteTimeout = _writeTimeout;

            _serialPort.Open();
        }

        public int BaudRate { get { return _serialPort.BaudRate; } set { _serialPort.BaudRate = value; } }
        public int DataBits { get { return _serialPort.DataBits; } set { _serialPort.DataBits = value; } }
        public int ReadTimeout { get { return _serialPort.ReadTimeout; } set { _serialPort.ReadTimeout = value; } }
        public int WriteTimeout { get { return _serialPort.WriteTimeout; } set { _serialPort.WriteTimeout = value; } }
        public Handshake Handshake { get { return _serialPort.Handshake; } set { _serialPort.Handshake = value; } }
        public Parity Parity { get { return _serialPort.Parity; } set { _serialPort.Parity = value; } }
        public string PortName { get { return _serialPort.PortName; } set { _serialPort.PortName = value; } }
        public StopBits StopBits { get { return _serialPort.StopBits; } set { _serialPort.StopBits = value; } }

        public string ReadSerial(char stopReadChar)
        {
            string data = string.Empty;
            bool _continue = true;

            while (_continue)
            {
                try
                {
                    char message = (char)_serialPort.ReadChar();
                    if (message == stopReadChar)
                    {
                        _continue = false;
                    }
                    else
                    {
                        data += message;
                    }
                }
                catch (TimeoutException)
                {

                }
            }

            return data;
        }
    }
}
