using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using com.ianlee.fezhatnode;
using Windows.Devices.AllJoyn;
using System.Threading.Tasks;
using Windows.Devices.Gpio;
using GHIElectronics.UAP.Shields;


// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

namespace FezHatAllJoynConsumer
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page, IfezhatnodeService
    {
        private bool _redLedState = false;
        private FEZHAT _fezhat;
        private readonly bool _isRunningOnPi;
        private AllJoynBusAttachment _busAttachment;
        private fezhatnodeProducer _producer;
        private fezhatnodeWatcher _watcher;
        private fezhatnodeConsumer _consumer;

        public MainPage()
        {
            this.InitializeComponent();
            this.Loaded += OnLoaded;

            _isRunningOnPi = Windows.Foundation.Metadata.ApiInformation.IsTypePresent("Windows.Devices.Gpio.GpioController");
            _isRunningOnPi = false;
            if (_isRunningOnPi)
            {
                Task.Run(async () => _fezhat = await FEZHAT.CreateAsync());
            }
        }

        private void OnLoaded(object sender, RoutedEventArgs e)
        {
            _busAttachment = new AllJoynBusAttachment();
            PopulateAboutData(_busAttachment);
            _producer = new fezhatnodeProducer(_busAttachment) {Service = this};
            _producer.Start();
            _watcher = new fezhatnodeWatcher(_busAttachment);
            _watcher.Added += OnAdded;
            _watcher.Start();
        }

        private async void OnAdded(fezhatnodeWatcher sender, AllJoynServiceInfo args)
        {
            if (_consumer == null)
            {
                var aboutData = await AllJoynAboutDataView.GetDataBySessionPortAsync(args.UniqueName, this._busAttachment, args.SessionPort);

            }
        }

        private void PopulateAboutData(AllJoynBusAttachment busAttachment)
        {
            busAttachment.AboutData.DateOfManufacture = DateTime.Now;
            busAttachment.AboutData.DefaultAppName = "Ian's FEZ HAT";
            busAttachment.AboutData.DefaultDescription = "Controls and monitors the devices on the FEZ HAT.";
            busAttachment.AboutData.SupportUrl = new Uri("http://ianlee.info");
        }

        public IAsyncOperation<fezhatnodeGetLightSensorValueResult> GetLightSensorValueAsync(AllJoynMessageInfo info)
        {
            return (
                Task.Run(() =>
                {
                    var lightLevel = 0.0;
                    if (_isRunningOnPi)
                    {
                        lightLevel = _fezhat.GetLightLevel();
                    }
                    return fezhatnodeGetLightSensorValueResult.CreateSuccessResult(lightLevel);
                }).AsAsyncOperation());
        }

        public IAsyncOperation<fezhatnodeGetTemperatureSensorValueResult> GetTemperatureSensorValueAsync(AllJoynMessageInfo info)
        {
            return (
                Task.Run(() =>
                {
                    var tempC = 0.0;
                    if (_isRunningOnPi)
                    {
                        tempC = _fezhat.GetTemperature();
                    }
                    return fezhatnodeGetTemperatureSensorValueResult.CreateSuccessResult(tempC, tempC * 1.8 + 32);
                }).AsAsyncOperation());
        }

        public IAsyncOperation<fezhatnodeSetRedLedStateResult> SetRedLedStateAsync(AllJoynMessageInfo info, bool interfaceMemberOn)
        {
            return (
                Task.Run(() =>
                {
                    SetRedLedState(interfaceMemberOn);
                    return fezhatnodeSetRedLedStateResult.CreateSuccessResult();
                }).AsAsyncOperation());
        }

        private void SetRedLedState(bool state)
        {
            _redLedState = state;
            if (_isRunningOnPi)
            {
                _fezhat.DIO24On = _redLedState;
            }
        }

        public IAsyncOperation<fezhatnodeGetRedLedStateResult> GetRedLedStateAsync(AllJoynMessageInfo info)
        {
            return (
                Task.Run(() =>
                {
                    return fezhatnodeGetRedLedStateResult.CreateSuccessResult(_redLedState);
                }).AsAsyncOperation());
        }

        public IAsyncOperation<fezhatnodeSetRgbLedD2ColorResult> SetRgbLedD2ColorAsync(AllJoynMessageInfo info, bool interfaceMemberOn, byte interfaceMemberRed, byte interfaceMemberGreen, byte interfaceMemberBlue)
        {
            return (
                Task.Run(() =>
                {
                    if (_isRunningOnPi)
                    {
                        _fezhat.D2.Color = new FEZHAT.Color(interfaceMemberRed, interfaceMemberGreen, interfaceMemberBlue);
                    }
                    return fezhatnodeSetRgbLedD2ColorResult.CreateSuccessResult();
                }).AsAsyncOperation());
        }

        public IAsyncOperation<fezhatnodeSetRgbLedD3ColorResult> SetRgbLedD3ColorAsync(AllJoynMessageInfo info, bool interfaceMemberOn, byte interfaceMemberRed, byte interfaceMemberGreen, byte interfaceMemberBlue)
        {
            return (
                Task.Run(() =>
                {
                    if (_isRunningOnPi)
                    {
                        _fezhat.D3.Color = new FEZHAT.Color(interfaceMemberRed, interfaceMemberGreen, interfaceMemberBlue);
                    }
                    return fezhatnodeSetRgbLedD3ColorResult.CreateSuccessResult();
                }).AsAsyncOperation());
        }

        private void RedLedSwitch_OnToggled(object sender, RoutedEventArgs e)
        {
            if (_consumer != null)
            {
                _consumer.SetRedLedStateAsync(uiRedLedSwitch.IsOn);
            }
        }
    }
}
