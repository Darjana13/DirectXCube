﻿using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace TemplateApp
{
    public partial class Form1 : Form
    {
        Timer timer = new Timer();

        public Form1()
        {
            InitializeComponent();

            timer.Interval = 30; // каждые 30 миллисекунд
           
            timer.Tick += new EventHandler((o, ev) =>
            {
                dxControl1.RotateByTime();
            });
            timer.Start();   
        }
        private void comboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            dxControl1.Invalidate();
        }

        private void Form1_Shown(object sender, EventArgs e)
        {
            dxControl1.EnablePaint = true;
            dxControl1.Invalidate();
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            timer.Stop();
        }
    }
}
