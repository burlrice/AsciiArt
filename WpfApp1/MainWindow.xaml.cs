using Microsoft.Win32;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Reflection;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Media.Imaging;

namespace WpfApp1
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window, INotifyPropertyChanged
    {
        Dictionary<string, List<string>> fonts { get; set; } = Ascii.FreeType.FixedWidthFonts;
        public List<string> Fonts => fonts.Select(f => f.Key).ToList().OrderBy(s => s).ToList();
        public string SelectedFont { get; set; }
        public string Data { get; set; }

        public string FileName { get; set; } = string.Join("\\", 
            Assembly.GetExecutingAssembly().Location.
            Split('\\').TakeWhile(s => s != "bin")
            .Union(new List<string>() { "Starry night.png" }));

        public BitmapSource Image { get; set; }

        public event PropertyChangedEventHandler PropertyChanged;

        public MainWindow()
        {
            InitializeComponent();
            SelectedFont = Fonts.Where(f => f == "Courier New").FirstOrDefault();
            PropertyChanged(this, new PropertyChangedEventArgs(nameof(SelectedFont)));
            ReloadImage(FileName);
        }

        private void OnBrowse(object sender, RoutedEventArgs e)
        {
            var title = this.FileName.Split('\\').LastOrDefault();

            var dialog = new OpenFileDialog() 
            {
                InitialDirectory = string.Join("\\", this.FileName.Split('\\').TakeWhile(s => s != title)),
                FileName = this.FileName, 
                Filter = "Image files|*.bmp;*.jpg;*.png;"
            };

            if (dialog.ShowDialog() == true)
            {
                ReloadImage(dialog.FileName);
            }
        }

        void ReloadImage(string fileName)
        {
            var img = new BitmapImage();
            var size = new Size(640, 480);

            img.BeginInit();
            img.UriSource = new Uri(fileName);
            img.EndInit();

            var aspectRatio = img.PixelWidth / (double)img.PixelHeight;
            var cx = Math.Min(img.PixelWidth, size.Width) / aspectRatio / img.PixelWidth;
            var cy = Math.Min(img.PixelHeight, size.Height) / aspectRatio / img.PixelHeight;

            Image = new TransformedBitmap(img, new ScaleTransform(cx, cy));

            FileName = fileName;
            PropertyChanged(this, new PropertyChangedEventArgs(nameof(FileName)));
            PropertyChanged(this, new PropertyChangedEventArgs(nameof(Image)));
        }

        private void OnFontSelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            Data = SelectedFont;
            PropertyChanged(this, new PropertyChangedEventArgs(nameof(Data)));
        }
    }
}
