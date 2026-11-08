# Log gia tri cam bien mau qua ST-Link, moi dong la mot vong cua task mau.
#
# Terminal 1: st-util
# Terminal 2: gdb -q -x color_log.gdb build/main.elf
# Dung log:   Ctrl+C trong gdb, go quit, Ctrl+C st-util, roi bam nut RESET tren board
#             (khong dung st-flash reset: chip clone nay se treo, firmware khong chay)
# File log:   color_log.txt, moi lan chay se ghi de len lan truoc
#
# c r g b : gia tri cam bien da tru hong ngoai, r va g da nhan he so CFG_COLOR_*_RAW_WEIGHT
# r% g% b%: ti le so voi c, day la so duoc so voi nguong CFG_COLOR_*
# color   : 0 chua nhan ra, 1 do, 2 xanh la, 3 xanh duong
# state   : 0 khoi tao, 1 cho LED, 2 dang lay mau, 3 da xac nhan, 4 loi, 5 cho vat (LED tat)
# h       : 0 khong co vat, 1 vat thap, 3 vat cao, 2 bat thuong (chi cam bien tren)

set pagination off
set confirm off
set logging file color_log.txt
set logging overwrite on
set logging enabled on
target extended-remote localhost:4242

dprintf sm_sort_update,"n=%lu c=%u r=%u g=%u b=%u | r%%=%u g%%=%u b%%=%u | color=%d state=%d id=0x%x err=%lu h=%d\n", color_debug.sample_count, color_debug.c, color_debug.r, color_debug.g, color_debug.b, (color_debug.c ? color_debug.r * 100 / color_debug.c : 0), (color_debug.c ? color_debug.g * 100 / color_debug.c : 0), (color_debug.c ? color_debug.b * 100 / color_debug.c : 0), color_debug.result, color_debug.state, color_debug.sensor_id, color_debug.error_count, height_debug.stable_bits

continue
