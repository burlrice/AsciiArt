using Ascii;
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
        public Generator Generator { get; set; } = new Generator();
        public string Data => string.Join("\r\n", Generator.Data);

        public BitmapSource? Image { get; set; }

        public event PropertyChangedEventHandler? PropertyChanged;

        public MainWindow()
        {
            var fileName = string.Join("\\",
                Assembly.GetExecutingAssembly().Location.
                Split('\\').TakeWhile(s => s != "bin")
                .Union(new List<string>() { "800px-ISO_C++_Logo.svg.png" }));

            InitializeComponent();
            Generator.FontFamily = Fonts.Where(f => f == "Courier New").FirstOrDefault();
            Generator.FontStyle = fonts.Where(f => f.Key == Generator.FontFamily).Select(f => f.Value.FirstOrDefault()).FirstOrDefault();
            Generator.PropertyChanged += (sender, e) => PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(Data)));
            ReloadImage(fileName);
        }

        private void OnBrowse(object sender, RoutedEventArgs e)
        {
            var title = Generator.FileName.Split('\\').LastOrDefault();
            var dialog = new OpenFileDialog() 
            {
                InitialDirectory = string.Join("\\", Generator.FileName.Split('\\').TakeWhile(s => s != title)),
                FileName = Generator.FileName, 
                Filter = "Image files|*.bmp;*.jpg;*.png;"
            };

            if (dialog.ShowDialog() == true)
            {
                ReloadImage(dialog.FileName);
            }
        }

        void ReloadImage(string fileName)
        {
            try
            {
                var img = new BitmapImage();
                var max = new Size(640, 480);

                img.BeginInit();
                img.UriSource = new Uri(fileName);
                img.EndInit();

                var scale = Math.Max(max.Width / img.PixelWidth, max.Height / img.PixelHeight);

                Image = new TransformedBitmap(img, new ScaleTransform(scale, scale));
                Generator.FileName = fileName;
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(Image)));
            }
            catch (Exception ex) { }
        }

        private void OnDrop(object sender, DragEventArgs e)
        {
            if (e.Data.GetDataPresent(DataFormats.FileDrop))
            {
                ReloadImage(((string[])e.Data.GetData(DataFormats.FileDrop)).FirstOrDefault());
            }
        }
    }
}
