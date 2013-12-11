using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace BansheeEngine
{
    public sealed class Texture2D : ScriptObject
    {
        public Texture2D(TextureFormat format, int width, int height, bool hasMipmaps = false, bool gammaCorrection = false)
        {
            Internal_CreateInstance(this, format, width, height, hasMipmaps, gammaCorrection);
        }

        ~Texture2D()
        {
            Internal_DestroyInstance(mCachedPtr);
        }

        [DllImport("__Internal")]
        private static extern void Internal_CreateInstance(Texture2D instance, TextureFormat format, int width, int height, bool hasMipmaps, bool gammaCorrection);

        [DllImport("__Internal")]
        private static extern void Internal_DestroyInstance(IntPtr nativeInstance);
    }
}
