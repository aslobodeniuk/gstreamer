// This file was generated by the Gtk# code generator.
// Any changes made will be lost if regenerated.

namespace Gst.Sdp {

	using System;
	using System.Collections;
	using System.Collections.Generic;
	using System.Runtime.InteropServices;

#region Autogenerated code
	[StructLayout(LayoutKind.Sequential)]
	public partial struct SDPAttribute : IEquatable<SDPAttribute> {

		public string Key;
		public string Value;

		public static Gst.Sdp.SDPAttribute Zero = new Gst.Sdp.SDPAttribute ();

		public static Gst.Sdp.SDPAttribute New(IntPtr raw) {
			if (raw == IntPtr.Zero)
				return Gst.Sdp.SDPAttribute.Zero;
			return (Gst.Sdp.SDPAttribute) Marshal.PtrToStructure (raw, typeof (Gst.Sdp.SDPAttribute));
		}

		[DllImport("gstsdp-1.0-0.dll", CallingConvention = CallingConvention.Cdecl)]
		static extern int gst_sdp_attribute_clear(IntPtr raw);

		public Gst.Sdp.SDPResult Clear() {
			IntPtr this_as_native = System.Runtime.InteropServices.Marshal.AllocHGlobal (System.Runtime.InteropServices.Marshal.SizeOf (this));
			System.Runtime.InteropServices.Marshal.StructureToPtr (this, this_as_native, false);
			int raw_ret = gst_sdp_attribute_clear(this_as_native);
			Gst.Sdp.SDPResult ret = (Gst.Sdp.SDPResult) raw_ret;
			ReadNative (this_as_native, ref this);
			System.Runtime.InteropServices.Marshal.FreeHGlobal (this_as_native);
			return ret;
		}

		[DllImport("gstsdp-1.0-0.dll", CallingConvention = CallingConvention.Cdecl)]
		static extern int gst_sdp_attribute_set(IntPtr raw, IntPtr key, IntPtr value);

		public Gst.Sdp.SDPResult Set(string key, string value) {
			IntPtr this_as_native = System.Runtime.InteropServices.Marshal.AllocHGlobal (System.Runtime.InteropServices.Marshal.SizeOf (this));
			System.Runtime.InteropServices.Marshal.StructureToPtr (this, this_as_native, false);
			IntPtr native_key = GLib.Marshaller.StringToPtrGStrdup (key);
			IntPtr native_value = GLib.Marshaller.StringToPtrGStrdup (value);
			int raw_ret = gst_sdp_attribute_set(this_as_native, native_key, native_value);
			Gst.Sdp.SDPResult ret = (Gst.Sdp.SDPResult) raw_ret;
			ReadNative (this_as_native, ref this);
			System.Runtime.InteropServices.Marshal.FreeHGlobal (this_as_native);
			GLib.Marshaller.Free (native_key);
			GLib.Marshaller.Free (native_value);
			return ret;
		}

		public Gst.Sdp.SDPResult Set(string key) {
			return Set (key, null);
		}

		static void ReadNative (IntPtr native, ref Gst.Sdp.SDPAttribute target)
		{
			target = New (native);
		}

		public bool Equals (SDPAttribute other)
		{
			return true && Key.Equals (other.Key) && Value.Equals (other.Value);
		}

		public override bool Equals (object other)
		{
			return other is SDPAttribute && Equals ((SDPAttribute) other);
		}

		public override int GetHashCode ()
		{
			return this.GetType ().FullName.GetHashCode () ^ Key.GetHashCode () ^ Value.GetHashCode ();
		}

		private static GLib.GType GType {
			get { return GLib.GType.Pointer; }
		}
#endregion
	}
}