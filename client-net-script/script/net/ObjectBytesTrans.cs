using UnityEngine;
using System.Collections;

public class ObjectBytesTrans
{
    public static object BytesToStruct(byte[] bytes, System.Type obj_type)
    {
        int size = System.Runtime.InteropServices.Marshal.SizeOf(obj_type);
        System.IntPtr buffer = System.Runtime.InteropServices.Marshal.AllocHGlobal(size);
        try
        {
            System.Runtime.InteropServices.Marshal.Copy(bytes, 0, buffer, size);
            return System.Runtime.InteropServices.Marshal.PtrToStructure(buffer, obj_type);
        }
        finally
        {
            System.Runtime.InteropServices.Marshal.FreeHGlobal(buffer);
        }
    }

    public static byte[] StructToBytes(object obj)
    {
        int size = System.Runtime.InteropServices.Marshal.SizeOf(obj);
        System.IntPtr buffer = System.Runtime.InteropServices.Marshal.AllocHGlobal(size);
        try
        {
            System.Runtime.InteropServices.Marshal.StructureToPtr(obj, buffer, false);
            byte[] bytes = new byte[size];
            System.Runtime.InteropServices.Marshal.Copy(buffer, bytes, 0, size);
            return bytes;
        }
        finally
        {
            System.Runtime.InteropServices.Marshal.FreeHGlobal(buffer);
        }
    }
}
