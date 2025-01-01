using Microsoft.Azure.Devices.Client;
using Newtonsoft.Json;
using System;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace SmartAgriculture
{
    class Program
    {

        private static readonly string connectionString = "HostName=yingying-first-iot-hub-name.azure-devices.net;DeviceId=yingDevice;SharedAccessKey=vLa90bxzUQxrimum5cgHP6NZRyyDGtkMzn+MzK81Xd4=";


        private static DeviceClient agricultureDeviceClient = null!;

        static Random random = new Random();

        static async Task Main(string[] args)
        {
            Console.WriteLine("Smart Farming Equipment Simulator Launched in...");
            agricultureDeviceClient = DeviceClient.CreateFromConnectionString(connectionString);

            var cts = new CancellationTokenSource();
            Console.CancelKeyPress += (s, e) =>
            {
                Console.WriteLine("The simulator stopped...");
                cts.Cancel();
                e.Cancel = true;
            };


            await SendSensorDataAsync(cts.Token);
        }

        private static async Task SendSensorDataAsync(CancellationToken token)
        {

            double soilMoisture = 40.0;
            double airTemperature = 25.0;
            double lightIntensity = 5000;
            double soilPH = 6.5;

            while (!token.IsCancellationRequested)
            {

                soilMoisture = GenerateSensorReading(soilMoisture, 20.0, 60.0, "SoilMoisture");
                airTemperature = GenerateSinusoidalValue(airTemperature, 10.0, 35.0, "Temperature");
                lightIntensity = GenerateSinusoidalValue(lightIntensity, 0.0, 100000.0, "LightIntensity");
                soilPH = GenerateSensorReading(soilPH, 5.5, 7.5, "SoilPH");


                var json = JsonConvert.SerializeObject(new
                {
                    soilMoisture,
                    airTemperature,
                    lightIntensity,
                    soilPH
                });


                var message = new Message(Encoding.ASCII.GetBytes(json))
                {
                    ContentType = "application/json",
                    ContentEncoding = "UTF-8"
                };

                await agricultureDeviceClient.SendEventAsync(message);
                Console.WriteLine($"[{DateTime.Now}]send message: {json}");

                await Task.Delay(5000); 
            }
        }

        private static double GenerateSensorReading(double currentValue, double min, double max, string sensorType)
        {
            double variation = sensorType switch
            {
                "SoilMoisture" => -0.5,
                "SoilPH" => 0.1,
                _ => 0.0
            };

            double newValue = currentValue + (variation + random.NextDouble() - 0.5);
            return Math.Max(min, Math.Min(max, newValue));
        }

        private static double GenerateSinusoidalValue(double currentValue, double min, double max, string sensorType)
        {
            double frequency = sensorType switch
            {
                "Temperature" => 0.01,
                "LightIntensity" => 0.02,
                _ => 0.0
            };

            double amplitude = (max - min) / 2;
            double midpoint = (max + min) / 2;
            double time = DateTime.Now.TimeOfDay.TotalSeconds;

            return midpoint + amplitude * Math.Sin(frequency * time);
        }
    }
}
