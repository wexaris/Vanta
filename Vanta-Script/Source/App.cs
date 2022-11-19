using System;
using System.Runtime.CompilerServices;

namespace Vanta
{
    public static class Internal
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Info(string text);
    }

    public class Entity
    {
        public Entity() {}
    }
}
