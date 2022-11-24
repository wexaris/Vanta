using System;

namespace Vanta {

    public static class Log {

        public static void Trace(string msg) {
            Internal.Log_Trace(msg);
        }

        public static void Info(string msg) {
            Internal.Log_Info(msg);
        }

        public static void Warn(string msg) {
            Internal.Log_Warn(msg);
        }

        public static void Error(string msg) {
            Internal.Log_Error(msg);
        }

        public static void Critical(string msg) {
            Internal.Log_Critical(msg);
        }
    }
}