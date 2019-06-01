palette = ["#000000", "#ffffff", "#880000", "#aaffee",
      "#cc44cc", "#00cc55", "#0000aa", "#eeee77",
      "#dd8855", "#664400", "#ff7777", "#333333",
      "#777777", "#aaff66", "#0088ff", "#bbbbbb"]

i = 0
for r,g,b in [(color[1:3], color[3:5], color[5:7]) for color in palette]:
    print(f'palette[{i}] = C2D_Color32(0x{r}, 0x{g}, 0x{b}, 0xFF);')
    i = i+1