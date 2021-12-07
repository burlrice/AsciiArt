using Ascii;
using GalaSoft.MvvmLight.Command;
using Microsoft.Win32;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Globalization;
using System.Linq;
using System.Reflection;
using System.Windows;
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
        //public List<string> Fonts => fonts.Select(f => f.Key).Where(f => !excludeFonts.Contains(f)).ToList().OrderBy(s => s).ToList();

        public List<string> Fonts => System.Windows.Media.Fonts.SystemFontFamilies
            .Select(f => f.Source)
            .Where(f => 
            {
                int height = 32;
                var a = MeasureString(f, height, ".....").Width;
                var b = MeasureString(f, height, "WWWWW").Width;

                return b > height && a == b;
            })
            .OrderBy(f => f)
            .ToList();

        public Generator Generator { get; set; } = new Generator();
        public string Data => string.Join("\r\n", Generator.Data);

        public BitmapSource? Image { get; set; }

        public RelayCommand Copy { get; set; }

        public event PropertyChangedEventHandler? PropertyChanged;

        public MainWindow()
        {
            var fileName = string.Join("\\",
                Assembly.GetExecutingAssembly().Location.
                Split('\\').TakeWhile(s => s != "bin")
                .Union(new List<string>() { "800px-ISO_C++_Logo.svg.png" }));

            Copy = new RelayCommand(OnCopyToClipboard);
            InitializeComponent();
            Generator.FontFamily = Fonts.Where(f => f == "Courier New").FirstOrDefault();
            Generator.FontStyle = fonts.Where(f => f.Key == Generator.FontFamily).Select(f => f.Value.FirstOrDefault()).FirstOrDefault();

            Generator.PropertyChanged += (sender, e) => 
            {
                if (e.PropertyName == nameof(Generator.FontFamily) || e.PropertyName == nameof(Generator.FontStyle) || Generator.FontAspectRatio == 1.0)
                {
                    var size = MeasureString(Generator.FontFamily, Generator.FontHeight, "12345\n12345\n12345\n12345\n12345");
                
                    Generator.FontAspectRatio = size.Width / size.Height;
                }

                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(Data)));
            };

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

        private Size MeasureString(string family, int height, string s)
        {
            var formattedText = new FormattedText(
                s,
                CultureInfo.CurrentCulture,
                FlowDirection.LeftToRight,
                new Typeface(family),
                height,
                Brushes.Black,
                new NumberSubstitution(),
                1);

            return new Size(formattedText.Width, formattedText.Height);
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

                var scale = Math.Min(1, Math.Max(max.Width / img.PixelWidth, max.Height / img.PixelHeight));

                Image = new TransformedBitmap(img, new ScaleTransform(scale, scale));
                Generator.FileName = fileName;
                PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(Image)));
                silderInputScale.Value = Math.Clamp(Image.PixelWidth, 10, max.Width);
            }
            catch (Exception ex) 
            {
                System.Diagnostics.Trace.WriteLine(ex.ToString());
            }
        }

        private void OnDrop(object sender, DragEventArgs e)
        {
            if (e.Data.GetDataPresent(DataFormats.FileDrop))
            {
                if (e.Data.GetData(DataFormats.FileDrop) is string[] data)
                {
                    ReloadImage(data.FirstOrDefault() ?? "");
                }
            }
        }

        private void OnCopyToClipboard()
        {
            Clipboard.SetText(Data);
            System.Diagnostics.Trace.WriteLine(Data);
        }
    }
}
