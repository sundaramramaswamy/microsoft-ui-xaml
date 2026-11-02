using System.Collections.Generic;
using System.Linq;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Data;
using Windows.Graphics;

namespace ListViewNeighborStatesApp
{
    public sealed class Item
    {
        public Item(string name, string group) { Name = name; Group = group; }
        public string Name { get; }
        public string Group { get; }
    }

    public sealed class ItemGroup : List<Item>
    {
        public ItemGroup(string key, IEnumerable<Item> items) : base(items) { Key = key; }
        public string Key { get; }
    }

    public sealed partial class MainWindow : Window
    {
        private static readonly (string Group, string[] Names)[] Data =
        {
            ("Fruits", new[] { "Apple", "Banana", "Cherry", "Date" }),
            ("Vegetables", new[] { "Carrot", "Leek", "Pea" }),
            ("Grains", new[] { "Oat", "Rice", "Wheat" }),
        };

        // Flat item ranges; Banana..Carrot crosses a group boundary.
        private static readonly ItemIndexRange[] InitialSelection =
        {
            new(1, 4),
            new(8, 1),
        };

        public MainWindow()
        {
            InitializeComponent();
            AppWindow.Resize(new SizeInt32(960, 800));
            Setup(Exp1List);
            Setup(Exp2List);
        }

        private static void Setup(ListView list)
        {
            var groups = Data.Select(g => new ItemGroup(g.Group, g.Names.Select(n => new Item(n, g.Group)))).ToList();
            list.ItemsSource = new CollectionViewSource { Source = groups, IsSourceGrouped = true }.View;
            foreach (var range in InitialSelection)
            {
                list.SelectRange(range);
            }
        }
    }
}
