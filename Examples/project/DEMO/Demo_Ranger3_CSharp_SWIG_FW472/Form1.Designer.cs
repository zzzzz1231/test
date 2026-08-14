namespace Demo.R3.SWIG.FW472.Winform
{
	partial class Form1
	{
		private System.ComponentModel.IContainer components = null;

		protected override void Dispose(bool disposing)
		{
			if (disposing && (components != null))
			{
				components.Dispose();
			}
			base.Dispose(disposing);
		}

		#region Windows Form Designer generated code

		private void InitializeComponent()
		{
			this.components = new System.ComponentModel.Container();
			this.splitContainerMain = new System.Windows.Forms.SplitContainer();
			this.splitContainerLeft = new System.Windows.Forms.SplitContainer();
			this.treeViewCameras = new System.Windows.Forms.TreeView();
			this.panelScan = new System.Windows.Forms.Panel();
			this.buttonScan = new System.Windows.Forms.Button();
			this.labelScanInfo = new System.Windows.Forms.Label();
			this.splitContainerRight = new System.Windows.Forms.SplitContainer();
			this.panelCameras = new System.Windows.Forms.Panel();
			this.panelCamera0 = new System.Windows.Forms.Panel();
			this.pictureBox0 = new System.Windows.Forms.PictureBox();
			this.panelCamera0Tools = new System.Windows.Forms.Panel();
			this.labelCamera0Info = new System.Windows.Forms.Label();
			this.buttonStop0 = new System.Windows.Forms.Button();
			this.buttonStart0 = new System.Windows.Forms.Button();
			this.buttonDisconnect0 = new System.Windows.Forms.Button();
			this.panelCamera1 = new System.Windows.Forms.Panel();
			this.pictureBox1 = new System.Windows.Forms.PictureBox();
			this.panelCamera1Tools = new System.Windows.Forms.Panel();
			this.labelCamera1Info = new System.Windows.Forms.Label();
			this.buttonStop1 = new System.Windows.Forms.Button();
			this.buttonStart1 = new System.Windows.Forms.Button();
			this.buttonDisconnect1 = new System.Windows.Forms.Button();
			this.panelCamera2 = new System.Windows.Forms.Panel();
			this.pictureBox2 = new System.Windows.Forms.PictureBox();
			this.panelCamera2Tools = new System.Windows.Forms.Panel();
			this.labelCamera2Info = new System.Windows.Forms.Label();
			this.buttonStop2 = new System.Windows.Forms.Button();
			this.buttonStart2 = new System.Windows.Forms.Button();
			this.buttonDisconnect2 = new System.Windows.Forms.Button();
			this.panelCamera3 = new System.Windows.Forms.Panel();
			this.pictureBox3 = new System.Windows.Forms.PictureBox();
			this.panelCamera3Tools = new System.Windows.Forms.Panel();
			this.labelCamera3Info = new System.Windows.Forms.Label();
			this.buttonStop3 = new System.Windows.Forms.Button();
			this.buttonStart3 = new System.Windows.Forms.Button();
			this.buttonDisconnect3 = new System.Windows.Forms.Button();
			this.panelParams = new System.Windows.Forms.Panel();
			this.panelParamButtons = new System.Windows.Forms.Panel();
			this.buttonApplyParam = new System.Windows.Forms.Button();
			this.buttonRefreshParam = new System.Windows.Forms.Button();
			this.labelParamTitle = new System.Windows.Forms.Label();
			this.propertyGridParams = new System.Windows.Forms.PropertyGrid();
			this.statusStrip = new System.Windows.Forms.StatusStrip();
			this.toolStripStatusLabel = new System.Windows.Forms.ToolStripStatusLabel();
			this.contextMenuStripTree = new System.Windows.Forms.ContextMenuStrip(this.components);
			this.menuItemOpen = new System.Windows.Forms.ToolStripMenuItem();
			this.menuItemClose = new System.Windows.Forms.ToolStripMenuItem();
			this.panelEmpty0 = new System.Windows.Forms.Panel();
			this.labelEmpty0 = new System.Windows.Forms.Label();
			this.panelEmpty1 = new System.Windows.Forms.Panel();
			this.labelEmpty1 = new System.Windows.Forms.Label();
			this.panelEmpty2 = new System.Windows.Forms.Panel();
			this.labelEmpty2 = new System.Windows.Forms.Label();
			this.panelEmpty3 = new System.Windows.Forms.Panel();
			this.labelEmpty3 = new System.Windows.Forms.Label();
			this.panelTopParams = new System.Windows.Forms.Panel();
			this.labelTopInfo = new System.Windows.Forms.Label();
			((System.ComponentModel.ISupportInitialize)(this.splitContainerMain)).BeginInit();
			this.splitContainerMain.Panel1.SuspendLayout();
			this.splitContainerMain.Panel2.SuspendLayout();
			this.splitContainerMain.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.splitContainerLeft)).BeginInit();
			this.splitContainerLeft.Panel1.SuspendLayout();
			this.splitContainerLeft.Panel2.SuspendLayout();
			this.splitContainerLeft.SuspendLayout();
			this.panelScan.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.splitContainerRight)).BeginInit();
			this.splitContainerRight.Panel1.SuspendLayout();
			this.splitContainerRight.Panel2.SuspendLayout();
			this.splitContainerRight.SuspendLayout();
			this.panelCameras.SuspendLayout();
			this.panelCamera0.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.pictureBox0)).BeginInit();
			this.panelCamera0Tools.SuspendLayout();
			this.panelCamera1.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
			this.panelCamera1Tools.SuspendLayout();
			this.panelCamera2.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.pictureBox2)).BeginInit();
			this.panelCamera2Tools.SuspendLayout();
			this.panelCamera3.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.pictureBox3)).BeginInit();
			this.panelCamera3Tools.SuspendLayout();
			this.panelParams.SuspendLayout();
			this.panelParamButtons.SuspendLayout();
			this.statusStrip.SuspendLayout();
			this.contextMenuStripTree.SuspendLayout();
			this.panelEmpty0.SuspendLayout();
			this.panelEmpty1.SuspendLayout();
			this.panelEmpty2.SuspendLayout();
			this.panelEmpty3.SuspendLayout();
			this.panelTopParams.SuspendLayout();
			this.SuspendLayout();

			// splitContainerMain
			this.splitContainerMain.Dock = System.Windows.Forms.DockStyle.Fill;
			this.splitContainerMain.Location = new System.Drawing.Point(0, 0);
			this.splitContainerMain.Name = "splitContainerMain";
			this.splitContainerMain.Panel1.Controls.Add(this.splitContainerLeft);
			this.splitContainerMain.Panel2.Controls.Add(this.splitContainerRight);
			this.splitContainerMain.Size = new System.Drawing.Size(1400, 800);
			this.splitContainerMain.SplitterDistance = 280;
			this.splitContainerMain.TabIndex = 0;

			// splitContainerLeft
			this.splitContainerLeft.Dock = System.Windows.Forms.DockStyle.Fill;
			this.splitContainerLeft.Location = new System.Drawing.Point(0, 0);
			this.splitContainerLeft.Name = "splitContainerLeft";
			this.splitContainerLeft.Orientation = System.Windows.Forms.Orientation.Horizontal;
			this.splitContainerLeft.Panel1.Controls.Add(this.treeViewCameras);
			this.splitContainerLeft.Panel1.Controls.Add(this.panelScan);
			this.splitContainerLeft.Panel2.Controls.Add(this.panelTopParams);
			this.splitContainerLeft.Size = new System.Drawing.Size(280, 800);
			this.splitContainerLeft.SplitterDistance = 420;
			this.splitContainerLeft.TabIndex = 0;

			// treeViewCameras
			this.treeViewCameras.Dock = System.Windows.Forms.DockStyle.Fill;
			this.treeViewCameras.Name = "treeViewCameras";
			this.treeViewCameras.TabIndex = 1;
			this.treeViewCameras.ContextMenuStrip = this.contextMenuStripTree;
			this.treeViewCameras.DoubleClick += new System.EventHandler(this.treeViewCameras_DoubleClick);

			// panelScan
			this.panelScan.Controls.Add(this.labelScanInfo);
			this.panelScan.Controls.Add(this.buttonScan);
			this.panelScan.Dock = System.Windows.Forms.DockStyle.Top;
			this.panelScan.Location = new System.Drawing.Point(0, 0);
			this.panelScan.Name = "panelScan";
			this.panelScan.Size = new System.Drawing.Size(280, 50);
			this.panelScan.TabIndex = 2;

			// buttonScan
			this.buttonScan.Location = new System.Drawing.Point(10, 10);
			this.buttonScan.Name = "buttonScan";
			this.buttonScan.Size = new System.Drawing.Size(100, 30);
			this.buttonScan.TabIndex = 0;
			this.buttonScan.Text = "扫描相机";
			this.buttonScan.UseVisualStyleBackColor = true;
			this.buttonScan.Click += new System.EventHandler(this.buttonScan_Click);

			// labelScanInfo
			this.labelScanInfo.AutoSize = true;
			this.labelScanInfo.Location = new System.Drawing.Point(120, 18);
			this.labelScanInfo.Name = "labelScanInfo";
			this.labelScanInfo.Size = new System.Drawing.Size(100, 15);
			this.labelScanInfo.TabIndex = 1;
			this.labelScanInfo.Text = "未找到相机";
			this.labelScanInfo.ForeColor = System.Drawing.Color.Gray;

			// panelTopParams
			this.panelTopParams.Controls.Add(this.labelTopInfo);
			this.panelTopParams.Dock = System.Windows.Forms.DockStyle.Fill;
			this.panelTopParams.Location = new System.Drawing.Point(0, 0);
			this.panelTopParams.Name = "panelTopParams";
			this.panelTopParams.Size = new System.Drawing.Size(280, 375);
			this.panelTopParams.TabIndex = 0;

			// labelTopInfo
			this.labelTopInfo.AutoSize = true;
			this.labelTopInfo.Font = new System.Drawing.Font("微软雅黑", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
			this.labelTopInfo.ForeColor = System.Drawing.Color.DarkSlateGray;
			this.labelTopInfo.Location = new System.Drawing.Point(10, 10);
			this.labelTopInfo.Name = "labelTopInfo";
			this.labelTopInfo.Size = new System.Drawing.Size(250, 120);
			this.labelTopInfo.TabIndex = 0;
			this.labelTopInfo.Text = "使用说明：\r\n1. 点击【扫描相机】查找设备\r\n2. 双击列表中的相机IP打开相机\r\n3. 系统自动分配到空闲显示区\r\n4. 点击【开始】进行图像采集\r\n5. 右侧【参数】面板可设置参数\r\n6. 点击【应用参数】使其生效";

			// splitContainerRight
			this.splitContainerRight.Dock = System.Windows.Forms.DockStyle.Fill;
			this.splitContainerRight.Location = new System.Drawing.Point(0, 0);
			this.splitContainerRight.Name = "splitContainerRight";
			this.splitContainerRight.Panel1.Controls.Add(this.panelCameras);
			this.splitContainerRight.Panel2.Controls.Add(this.panelParams);
			this.splitContainerRight.Size = new System.Drawing.Size(1116, 800);
			this.splitContainerRight.SplitterDistance = 750;
			this.splitContainerRight.TabIndex = 0;

			// panelCameras
			this.panelCameras.Controls.Add(this.panelCamera0);
			this.panelCameras.Controls.Add(this.panelCamera1);
			this.panelCameras.Controls.Add(this.panelCamera2);
			this.panelCameras.Controls.Add(this.panelCamera3);
			this.panelCameras.Dock = System.Windows.Forms.DockStyle.Fill;
			this.panelCameras.Location = new System.Drawing.Point(0, 0);
			this.panelCameras.Name = "panelCameras";
			this.panelCameras.Size = new System.Drawing.Size(750, 800);
			this.panelCameras.TabIndex = 0;
			this.panelCameras.Resize += new System.EventHandler(this.panelCameras_Resize);

			// panelCamera0
			this.panelCamera0.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(30)))), ((int)(((byte)(30)))), ((int)(((byte)(30)))));
			this.panelCamera0.Controls.Add(this.panelEmpty0);
			this.panelCamera0.Controls.Add(this.pictureBox0);
			this.panelCamera0.Controls.Add(this.panelCamera0Tools);
			this.panelCamera0.Location = new System.Drawing.Point(5, 5);
			this.panelCamera0.Name = "panelCamera0";
			this.panelCamera0.Size = new System.Drawing.Size(368, 390);
			this.panelCamera0.TabIndex = 0;
			this.panelCamera0.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;

			// pictureBox0
			this.pictureBox0.BackColor = System.Drawing.Color.Black;
			this.pictureBox0.Dock = System.Windows.Forms.DockStyle.Fill;
			this.pictureBox0.Location = new System.Drawing.Point(0, 40);
			this.pictureBox0.Name = "pictureBox0";
			this.pictureBox0.Size = new System.Drawing.Size(366, 348);
			this.pictureBox0.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
			this.pictureBox0.TabIndex = 0;
			this.pictureBox0.TabStop = false;

			// panelCamera0Tools
			this.panelCamera0Tools.Controls.Add(this.buttonDisconnect0);
			this.panelCamera0Tools.Controls.Add(this.buttonStop0);
			this.panelCamera0Tools.Controls.Add(this.buttonStart0);
			this.panelCamera0Tools.Controls.Add(this.labelCamera0Info);
			this.panelCamera0Tools.Dock = System.Windows.Forms.DockStyle.Top;
			this.panelCamera0Tools.Location = new System.Drawing.Point(0, 0);
			this.panelCamera0Tools.Name = "panelCamera0Tools";
			this.panelCamera0Tools.Size = new System.Drawing.Size(368, 40);
			this.panelCamera0Tools.TabIndex = 1;
			this.panelCamera0Tools.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(45)))), ((int)(((byte)(48)))));

			// labelCamera0Info
			this.labelCamera0Info.AutoSize = true;
			this.labelCamera0Info.ForeColor = System.Drawing.Color.White;
			this.labelCamera0Info.Location = new System.Drawing.Point(10, 14);
			this.labelCamera0Info.Name = "labelCamera0Info";
			this.labelCamera0Info.Size = new System.Drawing.Size(60, 15);
			this.labelCamera0Info.TabIndex = 0;
			this.labelCamera0Info.Text = "通道1";

			// buttonStart0
			this.buttonStart0.Location = new System.Drawing.Point(160, 8);
			this.buttonStart0.Name = "buttonStart0";
			this.buttonStart0.Size = new System.Drawing.Size(60, 25);
			this.buttonStart0.TabIndex = 1;
			this.buttonStart0.Text = "开始";
			this.buttonStart0.UseVisualStyleBackColor = true;
			this.buttonStart0.Enabled = false;
			this.buttonStart0.Click += new System.EventHandler(this.buttonStart0_Click);

			// buttonStop0
			this.buttonStop0.Location = new System.Drawing.Point(226, 8);
			this.buttonStop0.Name = "buttonStop0";
			this.buttonStop0.Size = new System.Drawing.Size(60, 25);
			this.buttonStop0.TabIndex = 2;
			this.buttonStop0.Text = "停止";
			this.buttonStop0.UseVisualStyleBackColor = true;
			this.buttonStop0.Enabled = false;
			this.buttonStop0.Click += new System.EventHandler(this.buttonStop0_Click);

			// buttonDisconnect0
			this.buttonDisconnect0.Location = new System.Drawing.Point(292, 8);
			this.buttonDisconnect0.Name = "buttonDisconnect0";
			this.buttonDisconnect0.Size = new System.Drawing.Size(60, 25);
			this.buttonDisconnect0.TabIndex = 3;
			this.buttonDisconnect0.Text = "断开";
			this.buttonDisconnect0.UseVisualStyleBackColor = true;
			this.buttonDisconnect0.Enabled = false;
			this.buttonDisconnect0.Click += new System.EventHandler(this.buttonDisconnect0_Click);

			// panelEmpty0
			this.panelEmpty0.Controls.Add(this.labelEmpty0);
			this.panelEmpty0.Dock = System.Windows.Forms.DockStyle.Fill;
			this.panelEmpty0.Location = new System.Drawing.Point(0, 40);
			this.panelEmpty0.Name = "panelEmpty0";
			this.panelEmpty0.Size = new System.Drawing.Size(366, 348);
			this.panelEmpty0.TabIndex = 2;
			this.panelEmpty0.Visible = true;

			// labelEmpty0
			this.labelEmpty0.AutoSize = true;
			this.labelEmpty0.ForeColor = System.Drawing.Color.Gray;
			this.labelEmpty0.Location = new System.Drawing.Point(150, 170);
			this.labelEmpty0.Name = "labelEmpty0";
			this.labelEmpty0.Size = new System.Drawing.Size(80, 15);
			this.labelEmpty0.TabIndex = 0;
			this.labelEmpty0.Text = "[空 闲]";
			this.labelEmpty0.Font = new System.Drawing.Font("微软雅黑", 12F, System.Drawing.FontStyle.Bold);

			// panelCamera1
			this.panelCamera1.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(30)))), ((int)(((byte)(30)))), ((int)(((byte)(30)))));
			this.panelCamera1.Controls.Add(this.panelEmpty1);
			this.panelCamera1.Controls.Add(this.pictureBox1);
			this.panelCamera1.Controls.Add(this.panelCamera1Tools);
			this.panelCamera1.Location = new System.Drawing.Point(379, 5);
			this.panelCamera1.Name = "panelCamera1";
			this.panelCamera1.Size = new System.Drawing.Size(368, 390);
			this.panelCamera1.TabIndex = 1;
			this.panelCamera1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;

			// pictureBox1
			this.pictureBox1.BackColor = System.Drawing.Color.Black;
			this.pictureBox1.Dock = System.Windows.Forms.DockStyle.Fill;
			this.pictureBox1.Location = new System.Drawing.Point(0, 40);
			this.pictureBox1.Name = "pictureBox1";
			this.pictureBox1.Size = new System.Drawing.Size(366, 348);
			this.pictureBox1.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
			this.pictureBox1.TabIndex = 0;
			this.pictureBox1.TabStop = false;

			// panelCamera1Tools
			this.panelCamera1Tools.Controls.Add(this.buttonDisconnect1);
			this.panelCamera1Tools.Controls.Add(this.buttonStop1);
			this.panelCamera1Tools.Controls.Add(this.buttonStart1);
			this.panelCamera1Tools.Controls.Add(this.labelCamera1Info);
			this.panelCamera1Tools.Dock = System.Windows.Forms.DockStyle.Top;
			this.panelCamera1Tools.Location = new System.Drawing.Point(0, 0);
			this.panelCamera1Tools.Name = "panelCamera1Tools";
			this.panelCamera1Tools.Size = new System.Drawing.Size(368, 40);
			this.panelCamera1Tools.TabIndex = 1;
			this.panelCamera1Tools.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(45)))), ((int)(((byte)(48)))));

			// labelCamera1Info
			this.labelCamera1Info.AutoSize = true;
			this.labelCamera1Info.ForeColor = System.Drawing.Color.White;
			this.labelCamera1Info.Location = new System.Drawing.Point(10, 14);
			this.labelCamera1Info.Name = "labelCamera1Info";
			this.labelCamera1Info.Size = new System.Drawing.Size(60, 15);
			this.labelCamera1Info.TabIndex = 0;
			this.labelCamera1Info.Text = "通道2";

			// buttonStart1
			this.buttonStart1.Location = new System.Drawing.Point(160, 8);
			this.buttonStart1.Name = "buttonStart1";
			this.buttonStart1.Size = new System.Drawing.Size(60, 25);
			this.buttonStart1.TabIndex = 1;
			this.buttonStart1.Text = "开始";
			this.buttonStart1.UseVisualStyleBackColor = true;
			this.buttonStart1.Enabled = false;
			this.buttonStart1.Click += new System.EventHandler(this.buttonStart1_Click);

			// buttonStop1
			this.buttonStop1.Location = new System.Drawing.Point(226, 8);
			this.buttonStop1.Name = "buttonStop1";
			this.buttonStop1.Size = new System.Drawing.Size(60, 25);
			this.buttonStop1.TabIndex = 2;
			this.buttonStop1.Text = "停止";
			this.buttonStop1.UseVisualStyleBackColor = true;
			this.buttonStop1.Enabled = false;
			this.buttonStop1.Click += new System.EventHandler(this.buttonStop1_Click);

			// buttonDisconnect1
			this.buttonDisconnect1.Location = new System.Drawing.Point(292, 8);
			this.buttonDisconnect1.Name = "buttonDisconnect1";
			this.buttonDisconnect1.Size = new System.Drawing.Size(60, 25);
			this.buttonDisconnect1.TabIndex = 3;
			this.buttonDisconnect1.Text = "断开";
			this.buttonDisconnect1.UseVisualStyleBackColor = true;
			this.buttonDisconnect1.Enabled = false;
			this.buttonDisconnect1.Click += new System.EventHandler(this.buttonDisconnect1_Click);

			// panelEmpty1
			this.panelEmpty1.Controls.Add(this.labelEmpty1);
			this.panelEmpty1.Dock = System.Windows.Forms.DockStyle.Fill;
			this.panelEmpty1.Location = new System.Drawing.Point(0, 40);
			this.panelEmpty1.Name = "panelEmpty1";
			this.panelEmpty1.Size = new System.Drawing.Size(366, 348);
			this.panelEmpty1.TabIndex = 2;
			this.panelEmpty1.Visible = true;

			// labelEmpty1
			this.labelEmpty1.AutoSize = true;
			this.labelEmpty1.ForeColor = System.Drawing.Color.Gray;
			this.labelEmpty1.Location = new System.Drawing.Point(150, 170);
			this.labelEmpty1.Name = "labelEmpty1";
			this.labelEmpty1.Size = new System.Drawing.Size(80, 15);
			this.labelEmpty1.TabIndex = 0;
			this.labelEmpty1.Text = "[空 闲]";
			this.labelEmpty1.Font = new System.Drawing.Font("微软雅黑", 12F, System.Drawing.FontStyle.Bold);

			// panelCamera2
			this.panelCamera2.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(30)))), ((int)(((byte)(30)))), ((int)(((byte)(30)))));
			this.panelCamera2.Controls.Add(this.panelEmpty2);
			this.panelCamera2.Controls.Add(this.pictureBox2);
			this.panelCamera2.Controls.Add(this.panelCamera2Tools);
			this.panelCamera2.Location = new System.Drawing.Point(5, 401);
			this.panelCamera2.Name = "panelCamera2";
			this.panelCamera2.Size = new System.Drawing.Size(368, 390);
			this.panelCamera2.TabIndex = 2;
			this.panelCamera2.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;

			// pictureBox2
			this.pictureBox2.BackColor = System.Drawing.Color.Black;
			this.pictureBox2.Dock = System.Windows.Forms.DockStyle.Fill;
			this.pictureBox2.Location = new System.Drawing.Point(0, 40);
			this.pictureBox2.Name = "pictureBox2";
			this.pictureBox2.Size = new System.Drawing.Size(366, 348);
			this.pictureBox2.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
			this.pictureBox2.TabIndex = 0;
			this.pictureBox2.TabStop = false;

			// panelCamera2Tools
			this.panelCamera2Tools.Controls.Add(this.buttonDisconnect2);
			this.panelCamera2Tools.Controls.Add(this.buttonStop2);
			this.panelCamera2Tools.Controls.Add(this.buttonStart2);
			this.panelCamera2Tools.Controls.Add(this.labelCamera2Info);
			this.panelCamera2Tools.Dock = System.Windows.Forms.DockStyle.Top;
			this.panelCamera2Tools.Location = new System.Drawing.Point(0, 0);
			this.panelCamera2Tools.Name = "panelCamera2Tools";
			this.panelCamera2Tools.Size = new System.Drawing.Size(368, 40);
			this.panelCamera2Tools.TabIndex = 1;
			this.panelCamera2Tools.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(45)))), ((int)(((byte)(48)))));

			// labelCamera2Info
			this.labelCamera2Info.AutoSize = true;
			this.labelCamera2Info.ForeColor = System.Drawing.Color.White;
			this.labelCamera2Info.Location = new System.Drawing.Point(10, 14);
			this.labelCamera2Info.Name = "labelCamera2Info";
			this.labelCamera2Info.Size = new System.Drawing.Size(60, 15);
			this.labelCamera2Info.TabIndex = 0;
			this.labelCamera2Info.Text = "通道3";

			// buttonStart2
			this.buttonStart2.Location = new System.Drawing.Point(160, 8);
			this.buttonStart2.Name = "buttonStart2";
			this.buttonStart2.Size = new System.Drawing.Size(60, 25);
			this.buttonStart2.TabIndex = 1;
			this.buttonStart2.Text = "开始";
			this.buttonStart2.UseVisualStyleBackColor = true;
			this.buttonStart2.Enabled = false;
			this.buttonStart2.Click += new System.EventHandler(this.buttonStart2_Click);

			// buttonStop2
			this.buttonStop2.Location = new System.Drawing.Point(226, 8);
			this.buttonStop2.Name = "buttonStop2";
			this.buttonStop2.Size = new System.Drawing.Size(60, 25);
			this.buttonStop2.TabIndex = 2;
			this.buttonStop2.Text = "停止";
			this.buttonStop2.UseVisualStyleBackColor = true;
			this.buttonStop2.Enabled = false;
			this.buttonStop2.Click += new System.EventHandler(this.buttonStop2_Click);

			// buttonDisconnect2
			this.buttonDisconnect2.Location = new System.Drawing.Point(292, 8);
			this.buttonDisconnect2.Name = "buttonDisconnect2";
			this.buttonDisconnect2.Size = new System.Drawing.Size(60, 25);
			this.buttonDisconnect2.TabIndex = 3;
			this.buttonDisconnect2.Text = "断开";
			this.buttonDisconnect2.UseVisualStyleBackColor = true;
			this.buttonDisconnect2.Enabled = false;
			this.buttonDisconnect2.Click += new System.EventHandler(this.buttonDisconnect2_Click);

			// panelEmpty2
			this.panelEmpty2.Controls.Add(this.labelEmpty2);
			this.panelEmpty2.Dock = System.Windows.Forms.DockStyle.Fill;
			this.panelEmpty2.Location = new System.Drawing.Point(0, 40);
			this.panelEmpty2.Name = "panelEmpty2";
			this.panelEmpty2.Size = new System.Drawing.Size(366, 348);
			this.panelEmpty2.TabIndex = 2;
			this.panelEmpty2.Visible = true;

			// labelEmpty2
			this.labelEmpty2.AutoSize = true;
			this.labelEmpty2.ForeColor = System.Drawing.Color.Gray;
			this.labelEmpty2.Location = new System.Drawing.Point(150, 170);
			this.labelEmpty2.Name = "labelEmpty2";
			this.labelEmpty2.Size = new System.Drawing.Size(80, 15);
			this.labelEmpty2.TabIndex = 0;
			this.labelEmpty2.Text = "[空 闲]";
			this.labelEmpty2.Font = new System.Drawing.Font("微软雅黑", 12F, System.Drawing.FontStyle.Bold);

			// panelCamera3
			this.panelCamera3.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(30)))), ((int)(((byte)(30)))), ((int)(((byte)(30)))));
			this.panelCamera3.Controls.Add(this.panelEmpty3);
			this.panelCamera3.Controls.Add(this.pictureBox3);
			this.panelCamera3.Controls.Add(this.panelCamera3Tools);
			this.panelCamera3.Location = new System.Drawing.Point(379, 401);
			this.panelCamera3.Name = "panelCamera3";
			this.panelCamera3.Size = new System.Drawing.Size(368, 390);
			this.panelCamera3.TabIndex = 3;
			this.panelCamera3.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;

			// pictureBox3
			this.pictureBox3.BackColor = System.Drawing.Color.Black;
			this.pictureBox3.Dock = System.Windows.Forms.DockStyle.Fill;
			this.pictureBox3.Location = new System.Drawing.Point(0, 40);
			this.pictureBox3.Name = "pictureBox3";
			this.pictureBox3.Size = new System.Drawing.Size(366, 348);
			this.pictureBox3.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
			this.pictureBox3.TabIndex = 0;
			this.pictureBox3.TabStop = false;

			// panelCamera3Tools
			this.panelCamera3Tools.Controls.Add(this.buttonDisconnect3);
			this.panelCamera3Tools.Controls.Add(this.buttonStop3);
			this.panelCamera3Tools.Controls.Add(this.buttonStart3);
			this.panelCamera3Tools.Controls.Add(this.labelCamera3Info);
			this.panelCamera3Tools.Dock = System.Windows.Forms.DockStyle.Top;
			this.panelCamera3Tools.Location = new System.Drawing.Point(0, 0);
			this.panelCamera3Tools.Name = "panelCamera3Tools";
			this.panelCamera3Tools.Size = new System.Drawing.Size(368, 40);
			this.panelCamera3Tools.TabIndex = 1;
			this.panelCamera3Tools.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(45)))), ((int)(((byte)(45)))), ((int)(((byte)(48)))));

			// labelCamera3Info
			this.labelCamera3Info.AutoSize = true;
			this.labelCamera3Info.ForeColor = System.Drawing.Color.White;
			this.labelCamera3Info.Location = new System.Drawing.Point(10, 14);
			this.labelCamera3Info.Name = "labelCamera3Info";
			this.labelCamera3Info.Size = new System.Drawing.Size(60, 15);
			this.labelCamera3Info.TabIndex = 0;
			this.labelCamera3Info.Text = "通道4";

			// buttonStart3
			this.buttonStart3.Location = new System.Drawing.Point(160, 8);
			this.buttonStart3.Name = "buttonStart3";
			this.buttonStart3.Size = new System.Drawing.Size(60, 25);
			this.buttonStart3.TabIndex = 1;
			this.buttonStart3.Text = "开始";
			this.buttonStart3.UseVisualStyleBackColor = true;
			this.buttonStart3.Enabled = false;
			this.buttonStart3.Click += new System.EventHandler(this.buttonStart3_Click);

			// buttonStop3
			this.buttonStop3.Location = new System.Drawing.Point(226, 8);
			this.buttonStop3.Name = "buttonStop3";
			this.buttonStop3.Size = new System.Drawing.Size(60, 25);
			this.buttonStop3.TabIndex = 2;
			this.buttonStop3.Text = "停止";
			this.buttonStop3.UseVisualStyleBackColor = true;
			this.buttonStop3.Enabled = false;
			this.buttonStop3.Click += new System.EventHandler(this.buttonStop3_Click);

			// buttonDisconnect3
			this.buttonDisconnect3.Location = new System.Drawing.Point(292, 8);
			this.buttonDisconnect3.Name = "buttonDisconnect3";
			this.buttonDisconnect3.Size = new System.Drawing.Size(60, 25);
			this.buttonDisconnect3.TabIndex = 3;
			this.buttonDisconnect3.Text = "断开";
			this.buttonDisconnect3.UseVisualStyleBackColor = true;
			this.buttonDisconnect3.Enabled = false;
			this.buttonDisconnect3.Click += new System.EventHandler(this.buttonDisconnect3_Click);

			// panelEmpty3
			this.panelEmpty3.Controls.Add(this.labelEmpty3);
			this.panelEmpty3.Dock = System.Windows.Forms.DockStyle.Fill;
			this.panelEmpty3.Location = new System.Drawing.Point(0, 40);
			this.panelEmpty3.Name = "panelEmpty3";
			this.panelEmpty3.Size = new System.Drawing.Size(366, 348);
			this.panelEmpty3.TabIndex = 2;
			this.panelEmpty3.Visible = true;

			// labelEmpty3
			this.labelEmpty3.AutoSize = true;
			this.labelEmpty3.ForeColor = System.Drawing.Color.Gray;
			this.labelEmpty3.Location = new System.Drawing.Point(150, 170);
			this.labelEmpty3.Name = "labelEmpty3";
			this.labelEmpty3.Size = new System.Drawing.Size(80, 15);
			this.labelEmpty3.TabIndex = 0;
			this.labelEmpty3.Text = "[空 闲]";
			this.labelEmpty3.Font = new System.Drawing.Font("微软雅黑", 12F, System.Drawing.FontStyle.Bold);

			// panelParams
			this.panelParams.Controls.Add(this.panelParamButtons);
			this.panelParams.Controls.Add(this.propertyGridParams);
			this.panelParams.Controls.Add(this.labelParamTitle);
			this.panelParams.Dock = System.Windows.Forms.DockStyle.Fill;
			this.panelParams.Location = new System.Drawing.Point(0, 0);
			this.panelParams.Name = "panelParams";
			this.panelParams.Size = new System.Drawing.Size(362, 800);
			this.panelParams.TabIndex = 0;

			// labelParamTitle
			this.labelParamTitle.Dock = System.Windows.Forms.DockStyle.Top;
			this.labelParamTitle.Font = new System.Drawing.Font("微软雅黑", 10F, System.Drawing.FontStyle.Bold);
			this.labelParamTitle.Location = new System.Drawing.Point(0, 0);
			this.labelParamTitle.Name = "labelParamTitle";
			this.labelParamTitle.Size = new System.Drawing.Size(362, 30);
			this.labelParamTitle.TabIndex = 0;
			this.labelParamTitle.Text = "相机参数";
			this.labelParamTitle.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			this.labelParamTitle.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(200)))), ((int)(((byte)(200)))), ((int)(((byte)(220)))));

			// propertyGridParams
			this.propertyGridParams.Dock = System.Windows.Forms.DockStyle.Fill;
			this.propertyGridParams.Location = new System.Drawing.Point(0, 30);
			this.propertyGridParams.Name = "propertyGridParams";
			this.propertyGridParams.Size = new System.Drawing.Size(362, 720);
			this.propertyGridParams.TabIndex = 1;
			this.propertyGridParams.PropertyValueChanged += new System.Windows.Forms.PropertyValueChangedEventHandler(this.propertyGridParams_PropertyValueChanged);

			// panelParamButtons
			this.panelParamButtons.Controls.Add(this.buttonApplyParam);
			this.panelParamButtons.Controls.Add(this.buttonRefreshParam);
			this.panelParamButtons.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.panelParamButtons.Location = new System.Drawing.Point(0, 750);
			this.panelParamButtons.Name = "panelParamButtons";
			this.panelParamButtons.Size = new System.Drawing.Size(362, 50);
			this.panelParamButtons.TabIndex = 2;

			// buttonApplyParam
			this.buttonApplyParam.Location = new System.Drawing.Point(190, 10);
			this.buttonApplyParam.Name = "buttonApplyParam";
			this.buttonApplyParam.Size = new System.Drawing.Size(80, 30);
			this.buttonApplyParam.TabIndex = 1;
			this.buttonApplyParam.Text = "应用参数";
			this.buttonApplyParam.UseVisualStyleBackColor = true;
			this.buttonApplyParam.Enabled = false;
			this.buttonApplyParam.Click += new System.EventHandler(this.buttonApplyParam_Click);

			// buttonRefreshParam
			this.buttonRefreshParam.Location = new System.Drawing.Point(90, 10);
			this.buttonRefreshParam.Name = "buttonRefreshParam";
			this.buttonRefreshParam.Size = new System.Drawing.Size(80, 30);
			this.buttonRefreshParam.TabIndex = 0;
			this.buttonRefreshParam.Text = "刷新参数";
			this.buttonRefreshParam.UseVisualStyleBackColor = true;
			this.buttonRefreshParam.Enabled = false;
			this.buttonRefreshParam.Click += new System.EventHandler(this.buttonRefreshParam_Click);

			// statusStrip
			this.statusStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripStatusLabel});
			this.statusStrip.Location = new System.Drawing.Point(0, 778);
			this.statusStrip.Name = "statusStrip";
			this.statusStrip.Size = new System.Drawing.Size(1400, 22);
			this.statusStrip.TabIndex = 1;

			// toolStripStatusLabel
			this.toolStripStatusLabel.Name = "toolStripStatusLabel";
			this.toolStripStatusLabel.Size = new System.Drawing.Size(100, 17);
			this.toolStripStatusLabel.Text = "就绪 - Ranger3 多相机演示";

			// contextMenuStripTree
			this.contextMenuStripTree.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.menuItemOpen,
            this.menuItemClose});
			this.contextMenuStripTree.Name = "contextMenuStripTree";
			this.contextMenuStripTree.Size = new System.Drawing.Size(150, 48);
			this.contextMenuStripTree.Opening += new System.ComponentModel.CancelEventHandler(this.contextMenuStripTree_Opening);

			// menuItemOpen
			this.menuItemOpen.Name = "menuItemOpen";
			this.menuItemOpen.Size = new System.Drawing.Size(149, 22);
			this.menuItemOpen.Text = "打开相机";
			this.menuItemOpen.Click += new System.EventHandler(this.menuItemOpen_Click);

			// menuItemClose
			this.menuItemClose.Name = "menuItemClose";
			this.menuItemClose.Size = new System.Drawing.Size(149, 22);
			this.menuItemClose.Text = "关闭相机";
			this.menuItemClose.Click += new System.EventHandler(this.menuItemClose_Click);

			// Form1
			this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 15F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(1400, 800);
			this.Controls.Add(this.splitContainerMain);
			this.Controls.Add(this.statusStrip);
			this.Font = new System.Drawing.Font("微软雅黑", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
			this.MainMenuStrip = null;
			this.Name = "Form1";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
			this.Text = "Ranger3 多相机演示 - SwigRanger3.FW472";
			this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Form1_FormClosing);
			this.Load += new System.EventHandler(this.Form1_Load);
			this.splitContainerMain.Panel1.ResumeLayout(false);
			this.splitContainerMain.Panel2.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.splitContainerMain)).EndInit();
			this.splitContainerMain.ResumeLayout(false);
			this.splitContainerLeft.Panel1.ResumeLayout(false);
			this.splitContainerLeft.Panel2.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.splitContainerLeft)).EndInit();
			this.splitContainerLeft.ResumeLayout(false);
			this.panelScan.ResumeLayout(false);
			this.panelScan.PerformLayout();
			this.splitContainerRight.Panel1.ResumeLayout(false);
			this.splitContainerRight.Panel2.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.splitContainerRight)).EndInit();
			this.splitContainerRight.ResumeLayout(false);
			this.panelCameras.ResumeLayout(false);
			this.panelCamera0.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.pictureBox0)).EndInit();
			this.panelCamera0Tools.ResumeLayout(false);
			this.panelCamera0Tools.PerformLayout();
			this.panelCamera1.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
			this.panelCamera1Tools.ResumeLayout(false);
			this.panelCamera1Tools.PerformLayout();
			this.panelCamera2.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.pictureBox2)).EndInit();
			this.panelCamera2Tools.ResumeLayout(false);
			this.panelCamera2Tools.PerformLayout();
			this.panelCamera3.ResumeLayout(false);
			((System.ComponentModel.ISupportInitialize)(this.pictureBox3)).EndInit();
			this.panelCamera3Tools.ResumeLayout(false);
			this.panelCamera3Tools.PerformLayout();
			this.panelParams.ResumeLayout(false);
			this.panelParamButtons.ResumeLayout(false);
			this.statusStrip.ResumeLayout(false);
			this.statusStrip.PerformLayout();
			this.contextMenuStripTree.ResumeLayout(false);
			this.panelEmpty0.ResumeLayout(false);
			this.panelEmpty0.PerformLayout();
			this.panelEmpty1.ResumeLayout(false);
			this.panelEmpty1.PerformLayout();
			this.panelEmpty2.ResumeLayout(false);
			this.panelEmpty2.PerformLayout();
			this.panelEmpty3.ResumeLayout(false);
			this.panelEmpty3.PerformLayout();
			this.panelTopParams.ResumeLayout(false);
			this.panelTopParams.PerformLayout();
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.SplitContainer splitContainerMain;
		private System.Windows.Forms.SplitContainer splitContainerLeft;
		private System.Windows.Forms.SplitContainer splitContainerRight;
		private System.Windows.Forms.Panel panelCameras;
		private System.Windows.Forms.Panel panelParams;
		private System.Windows.Forms.TreeView treeViewCameras;
		private System.Windows.Forms.Panel panelScan;
		private System.Windows.Forms.Button buttonScan;
		private System.Windows.Forms.Label labelScanInfo;
		private System.Windows.Forms.PropertyGrid propertyGridParams;
		private System.Windows.Forms.Label labelParamTitle;
		private System.Windows.Forms.Panel panelParamButtons;
		private System.Windows.Forms.Button buttonApplyParam;
		private System.Windows.Forms.Button buttonRefreshParam;
		private System.Windows.Forms.StatusStrip statusStrip;
		private System.Windows.Forms.ToolStripStatusLabel toolStripStatusLabel;
		private System.Windows.Forms.ContextMenuStrip contextMenuStripTree;
		private System.Windows.Forms.ToolStripMenuItem menuItemOpen;
		private System.Windows.Forms.ToolStripMenuItem menuItemClose;

		private System.Windows.Forms.Panel panelCamera0;
		private System.Windows.Forms.PictureBox pictureBox0;
		private System.Windows.Forms.Panel panelCamera0Tools;
		private System.Windows.Forms.Label labelCamera0Info;
		private System.Windows.Forms.Button buttonStart0;
		private System.Windows.Forms.Button buttonStop0;
		private System.Windows.Forms.Button buttonDisconnect0;
		private System.Windows.Forms.Panel panelEmpty0;
		private System.Windows.Forms.Label labelEmpty0;

		private System.Windows.Forms.Panel panelCamera1;
		private System.Windows.Forms.PictureBox pictureBox1;
		private System.Windows.Forms.Panel panelCamera1Tools;
		private System.Windows.Forms.Label labelCamera1Info;
		private System.Windows.Forms.Button buttonStart1;
		private System.Windows.Forms.Button buttonStop1;
		private System.Windows.Forms.Button buttonDisconnect1;
		private System.Windows.Forms.Panel panelEmpty1;
		private System.Windows.Forms.Label labelEmpty1;

		private System.Windows.Forms.Panel panelCamera2;
		private System.Windows.Forms.PictureBox pictureBox2;
		private System.Windows.Forms.Panel panelCamera2Tools;
		private System.Windows.Forms.Label labelCamera2Info;
		private System.Windows.Forms.Button buttonStart2;
		private System.Windows.Forms.Button buttonStop2;
		private System.Windows.Forms.Button buttonDisconnect2;
		private System.Windows.Forms.Panel panelEmpty2;
		private System.Windows.Forms.Label labelEmpty2;

		private System.Windows.Forms.Panel panelCamera3;
		private System.Windows.Forms.PictureBox pictureBox3;
		private System.Windows.Forms.Panel panelCamera3Tools;
		private System.Windows.Forms.Label labelCamera3Info;
		private System.Windows.Forms.Button buttonStart3;
		private System.Windows.Forms.Button buttonStop3;
		private System.Windows.Forms.Button buttonDisconnect3;
		private System.Windows.Forms.Panel panelEmpty3;
		private System.Windows.Forms.Label labelEmpty3;
		private System.Windows.Forms.Panel panelTopParams;
		private System.Windows.Forms.Label labelTopInfo;
	}
}
