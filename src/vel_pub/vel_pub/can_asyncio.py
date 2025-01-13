import asyncio
import can
import math
import time

def print_message(msg):
    """Regular callback function. Can also be a coroutine."""
    return

async def main():
    can0 = can.Bus('can0', bustype='socketcan', bitrate = 500000)
    reader = can.AsyncBufferedReader()
    logger = can.Logger('logfile.asc')

    listeners = [
        print_message,  # Callback function
        reader,         # AsyncBufferedReader() listener
        logger          # Regular Listener object
    ]
    # Create Notifier with an explicit loop to use for scheduling of callbacks
    loop = asyncio.get_event_loop()
    notifier = can.Notifier(can0, listeners, loop=loop)
    while True: 
        msg = await reader.get_message() 
        if msg.arbitration_id == 533: # 0x215
            randnum = msg.data[1]
            shift = msg.data[2]
            result = math.floor(randnum/(2**shift))
            can0.send(can.Message(arbitration_id = 405, data = [2, 0, 0, result, 0, 0, 0, 0], is_extended_id = False)) # 0x195 
            can0.send(can.Message(arbitration_id = 661, data = [3, 0, 0, 0, 0, 0, 0, 0], is_extended_id = False)) # 0x295 
            if msg.data[0] == 3:
                break   
            await asyncio.sleep(0.01)  
            
    a = 0
    while a<500:
        # print(result)
        can0.send(can.Message(arbitration_id = 405, data = [2, 0, 0, result, 0, 0, 0, 0], is_extended_id = False)) # 0x195 
        can0.send(can.Message(arbitration_id = 661, data = [83, 120, 0, 120, 0, 1, 0, 0], is_extended_id = False)) # 0x295
        a = a + 1
        await asyncio.sleep(0.01)

    await reader.get_message()
    # print('Done!')

    # Clean-up
    notifier.stop()
    can0.shutdown()

# Get the default event loop
loop = asyncio.get_event_loop()
# Run until main coroutine finishes
loop.run_until_complete(main())
loop.close()