using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ROSA_DEMO_WPF
{
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
