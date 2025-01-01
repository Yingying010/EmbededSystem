using Microsoft.Azure.Devices.Client;
using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace AzureIoT.smartNappyDevice
{
    class SmartNappy
    {
        //Connection string for device to cloud messaging
        private static readonly string connectionString_IoTHub = "<Inser your Connection String here>";

        //Device Client
        static DeviceClient smartNappyClient;

        //Random Generator
        static Random random = new Random();

        //nappy sensor details
        // we can imagine our smart nappy to have three sensors: 
        // one to measure the baby's skin temperature
        const double babyTemperature_min = 35;
        const double babyTemperature_max = 37;
        static double babyTemperature = 36;
        //one to measure the humidity of the nappy, 
        const double nappyHumidity_min = 80;
        const double nappyHumidity_max = 120;
        static double nappyHumidity = 80;
        // One to measure the fullness of the nappy by recording the stretch on the nappy's fabric 
        const double nappyElasticTension_min = 15;
        const double nappyElasticTension_max = 35;
        static double nappyElasticTension = 20;

        static void Main(string[] args)
        {
            var cts = new CancellationTokenSource();
            Console.WriteLine("Press CTRL+C to stop the simulation");
            Console.CancelKeyPress += (s,e)=>
            {
                Console.WriteLine("Stopping the Application....");
                cts.Cancel();
                e.Cancel = true;
            };

            smartNappyClient = DeviceClient.CreateFromConnectionString(connectionString_IoTHub);

            SendMessagesToIoTHub(cts.Token);

            Console.ReadLine();

        }

        private static async void SendMessagesToIoTHub(CancellationToken token)
        {
            while (!token.IsCancellationRequested)
            {
                nappyHumidity = GenerateSensorReading(nappyHumidity, nappyHumidity_min, nappyHumidity_max);
                nappyElasticTension = GenerateSensorReading(nappyElasticTension, nappyElasticTension_min, nappyElasticTension_max);
                babyTemperature = GenerateSensorReading(nappyElasticTension, nappyElasticTension_min, nappyElasticTension_max);

                var json = CreateJSON(babyTemperature, nappyHumidity, nappyElasticTension);
                var message = CreateMessage(json);
                await smartNappyClient.SendEventAsync(message);
                Console.WriteLine($"Sending message at {DateTime.Now} and Message : {json}");
                await Task.Delay(5000);
            }
        }

        // the generate sensor reading will be crucial in the simulations of your device. Here, you are describing the behaviour of the simulated data to be sent to the Hub. 
        private static double GenerateSensorReading(double currentValue, double min, double max)
        {
            double percentage = 5; // 5%

            // generate a new value based on the previous supplied value
            // The new value will be calculated to be within the threshold specified by the "percentage" variable from the original number.
            // The value will also always be within the the specified "min" and "max" values.
            double value = currentValue * (1 + ((percentage / 100) * (2 * random.NextDouble() - 1)));

            value = Math.Max(value, min);
            value = Math.Min(value, max);

            return value;
        }

        private static string CreateJSON(double tempdata, double humidity, double strain)
        {
            var data = new
            {
                temp = tempdata,
                humidity = humidity,
                elastic_strain = strain
            };
            return JsonConvert.SerializeObject(data);
        }

        private static Message CreateMessage(string jsonObject)
        {
            var message = new Message(Encoding.ASCII.GetBytes(jsonObject));

            // MESSAGE CONTENT TYPE
            message.ContentType = "application/json";
            message.ContentEncoding = "UTF-8";

            return message;
        }
    }
}
