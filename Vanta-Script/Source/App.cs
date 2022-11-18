using System;

namespace Vanta
{
    public class Main
    {
        public float Float = 5.0f;

        public Main()
        {
            Console.WriteLine("Hello world!");
            PrintFloatVar();
            IncrementFloatVar(2.0f);
            PrintFloatVar();
        }

        public void PrintFloatVar()
        {
            Console.WriteLine("Float = {0:F}", Float);
        }

        private void IncrementFloatVar(float value)
        {
            Float += value;
        }
    }
}
