using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;

namespace ROSA_DEMO_WPF
{
    class MainViewModel : BindableBase
    {
        SerialReader sr; 


        public MainViewModel()
        {
            SerialPorts = new ObservableCollection<string>(SerialPort.GetPortNames().ToList());
            SelectedSerialPort = SerialPorts.First();

            sr = new SerialReader(SelectedSerialPort);

            Task.Run(() =>
            {
                int pot = 0;
                int headlights = 0;
                int brakes = 0;
                int brake_active = 0;
                int steeringLeft = 0;
                int steeringRight = 0;

                while (true)
                {
                    Task.Delay(10);

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

                        Application.Current.Dispatcher.Invoke(new Action(() =>
                        {
                            PotValue = pot;
                            Headlights = headlights != 0;
                            Brakes = brakes != 0;
                            BrakeActive = brake_active != 0;

                            if (steeringLeft == 1)
                                Steering = "Left";
                            else if (steeringRight == 1)
                                Steering = "Right";
                            else Steering = "Neutral";
                        }));
                    }
                    catch (Exception e)
                    {
                        Debug.WriteLine(e.Message);
                    }
                }
            });
        }

        private ObservableCollection<string> _SerialPorts = new ObservableCollection<string>();
        public ObservableCollection<string> SerialPorts
        {
            get { return _SerialPorts; }
            set { SetProperty(ref _SerialPorts, value); }
        }

        private string _SelectedSerialPort = "COM3";
        public string SelectedSerialPort
        {
            get { return _SelectedSerialPort; }
            set
            {
                SetProperty(ref _SelectedSerialPort, value);
                if(sr != null)
                {
                    sr.PortName = value;
                }
            }
        }

        private int _PotValue = 0;
        public int PotValue
        {
            get { return _PotValue; }
            set { SetProperty(ref _PotValue, value); }
        }

        private bool _Headlights = false;
        public bool Headlights
        {
            get { return _Headlights; }
            set { SetProperty(ref _Headlights, value); }
        }

        private bool _Brakes = false;
        public bool Brakes
        {
            get { return _Brakes; }
            set { SetProperty(ref _Brakes, value); }
        }

        private bool _BrakeActive = false;
        public bool BrakeActive
        {
            get { return _BrakeActive; }
            set { SetProperty(ref _BrakeActive, value); }
        }

        private string _Steering = "Neutral";
        public string Steering
        {
            get { return _Steering; }
            set { SetProperty(ref _Steering, value); }
        }
    }
}
