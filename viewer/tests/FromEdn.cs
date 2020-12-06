using System.Collections.Generic;
using System.Text.Json;
using NUnit.Framework;
using JsonSerializer = System.Text.Json.JsonSerializer;

namespace net_draw.tests {
    
    public class FromEdn {
        public static string obj_edn = @"
{'kind': 'obj', 'proto': 
  {'kind': 'proto', 'name': 'StripWholeLSB', 'inputs': [['List', 'In1']], 'outputs': [['List', 'WholeLSB'], ['List', 'Out2']], 'attrs': {'TreeGroup': 'BotControl'}}, 
  'behavior': [
    {'kind': 'proto', 'name': 'Input', 'inputs': [], 'outputs': [['List', 'In1']], 'attrs': {}, 'gui': [24, 32]}, 
    {'kind': 'proto', 'name': 'Output', 'inputs': [['List', 'WholeLSB']], 'outputs': [], 'attrs': {}, 'gui': [88, 31]}, 
    {'kind': 'proto', 'name': 'Output:A', 'inputs': [['List', 'Out2']], 'outputs': [], 'attrs': {}, 'gui': [88, 40]}, 
    {'kind': 'proto', 'name': 'ListIn', 'inputs': [['List', 'In']], 'outputs': [['Variant', 'Out1'], ['Variant', 'Out2']], 'attrs': {}, 'gui': [29, 30]}, 
    {'kind': 'proto', 'name': 'ListOut', 'inputs': [['Variant', 'In1'], ['Variant', 'In2']], 'outputs': [['List', 'Out']], 'attrs': {}, 'gui': [73, 38]}, 
    {'kind': 'proto', 'name': 'ListOut:A', 'inputs': [['Variant', 'In1'], ['Variant', 'In2']], 'outputs': [['List', 'Out']], 'attrs': {}, 'gui': [74, 29]}, 
    {'kind': 'proto', 'name': 'StripWholeLSB', 'inputs': [['Variant', 'In1']], 'outputs': [['Variant', 'WholeLSB'], ['Variant', 'Out2']], 'attrs': {}, 'gui': [47, 38]}, 
    {'kind': 'proto', 'name': 'StripWholeLSB:A', 'inputs': [['Variant', 'In1']], 'outputs': [['Variant', 'WholeLSB'], ['Variant', 'Out2']], 'attrs': {}, 'gui': [47, 29]}]}

".Replace("'", "\"").Trim();
       
        
        [Test]
        public void Test() {
            
            var options = new JsonSerializerOptions {
                PropertyNameCaseInsensitive = true,
            };
            var obj = JsonSerializer.Deserialize<ObjectDef>(obj_edn, options);
            
            Assert.AreEqual("StripWholeLSB", obj.Proto.Name);


        }


        
    }
    
    
        public class Pos {
            public int X { get; set; }
            public int Y { get; set; }
        }

        public class ObjectDef {
            public Proto Proto { get; set; }
            public List<Proto> Behavior { get; set; }
            public List<Transport> Net { get; set; }
            public List<TextBox> Texts { get; set; }
            
        }

        public class Proto {
            
            public string Name { get; set; }
            public List<int> Gui { get; set; }
            public List<List<string>> Inputs { get; set; }
            public List<List<string>> Outputs { get; set; }

            public override string ToString() {
                return $"Proto {Name}";
            }
        }

        public class TextBox {
            public List<int> Gui { get; set; }
            public string Text { get; set; }
        }

        public class Transport {
            public string Input { get; set; }
            public string Output { get; set; }
            public List<List<int>> Gui { get; set; }
        }
}