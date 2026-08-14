// In order to release image data
[System.Runtime.InteropServices.DllImport("gdi32.dll")]
public static extern bool DeleteObject(IntPtr hObject);


private HalconDotNet.HImage Icon2HalconFloat(CSImgT imgTable)
{
	var RawImage = new HalconDotNet.HImage();
	var reduceImage = new HalconDotNet.HImage();
	float[] rangeData = imgTable.getDataFloat(DataNames.RAN_CAL);
	unsafe
	{
		fixed (float* pSrc = rangeData)
		{
			RawImage.GenImage1("real", Convert.ToInt32(imgTable.get_cols()), Convert.ToInt32(imgTable.get_rows()), new System.IntPtr(pSrc));
			HalconDotNet.HRegion hRegion = RawImage.Threshold(0.000001, 1000);
			reduceImage = RawImage.ReduceDomain(hRegion);
		}
	}

	return reduceImage;
}

private HalconDotNet.HImage Icon2HalconByte(CSImgT imgTable)
{
	var RawImage = new HalconDotNet.HImage();
	var reduceImage = new HalconDotNet.HImage();
	byte[] rangeData = imgTable.getDataByte(DataNames.REF_CAL);
	unsafe
	{
		fixed (byte* pSrc = rangeData)
		{
			RawImage.GenImage1("byte", Convert.ToInt32(imgTable.get_cols()), Convert.ToInt32(imgTable.get_rows()), new System.IntPtr(pSrc));
			HalconDotNet.HRegion hRegion = RawImage.Threshold(0.000001, 1000);
			reduceImage = RawImage.ReduceDomain(hRegion);
		}
	}

	return reduceImage;
}

private void Icon2HalconDisplay(CSImgT imgTable, DataNames dn)
{
	if (!(dn == DataNames.RAN_CAL || dn == DataNames.REF_CAL))
	{
		return;
	}

	var reduceImage = (dn == DataNames.REF_CAL ? Icon2HalconByte(imgTable) : Icon2HalconFloat(imgTable));

	// show image in halcon window;
	HalconDotNet.HTuple hWidth, hHeight = new HalconDotNet.HTuple();
	reduceImage.GetImageSize(out hWidth, out hHeight);
	hWindowControl1.HalconWindow.SetPart(0, 0, hHeight.I, hWidth.I);
	hWindowControl1.HalconWindow.DispImage(reduceImage);
}


//调用示例：

CSImgT imgTable = new CSImgT();
if (Tri(m_ip).getImageData(imgTable) == CAM_STATUS.All_OK)
{
	// 读取图片 convert image to show
	switch (mode)
	{
		case DataNames.REF_CAL:
			Icon2HalconDisplay(imgTable, DataNames.REF_CAL);
			break;
		case DataNames.RAN_CAL:
			Icon2HalconDisplay(imgTable, DataNames.RAN_CAL);
			break;
		default:
			imgTable.Dispose();
			return;
	}

}
imgTable.Dispose();
GC.Collect();
GC.WaitForPendingFinalizers();
