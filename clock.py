f = 16000000
prescaler_options = [1,8,32,64,128,256,1024]

target_time = 0.005
print('limit (s)=', 2**16*target_time)

for e in prescaler_options:
    counts = target_time*f/e
    if counts < 2**8:
        print('prescaler=',e,
              ' counter=',counts,
              ' time=', int(counts)*e/f) # counts
