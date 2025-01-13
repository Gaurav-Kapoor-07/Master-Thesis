import can
import math
import time
import matplotlib.pyplot as plt


class mattro:

	def __init__(self):
	
		self.bus = can.interface.Bus('can0', bustype = 'socketcan', bitrate = 500000)
		self.result = 0
		self.vel_dict = []

	def activate(self):
		
		while True: 
			tpdo = self.bus.recv() 	
			if tpdo.arbitration_id == 533: # 0x215
				randnum = tpdo.data[1]
				shift = tpdo.data[2]
				self.result = math.floor(randnum/(2**shift))
				self.bus.send(can.Message(arbitration_id = 405, data = [2, 0, 0, self.result, 0, 0, 0, 0], is_extended_id = False)) # 0x195	
				self.bus.send(can.Message(arbitration_id = 661, data = [3, 0, 0, 0, 0, 0, 0, 0], is_extended_id = False)) # 0x295	
				if tpdo.data[0] == 3:
					return 
				time.sleep(0.01) 	

	def cal_hex(self, n):

		l = [] 		
		for i in range(len(n)): 
			hex_n = hex(round(n[i]))
			hex_n = hex_n.replace('x', '')
			hex_n = '00' + hex_n
			l.append([int(hex_n[-2:], 16), int(hex_n[:len(hex_n) - 2], 16)]) 
		return l

	def convert_rpm_to_lin_vel(self, rpm):

		v_lin = [0]*len(rpm) 
		for i in range(len(rpm)): 
			v_lin[i] = rpm[i]/393.405769951 
		return v_lin

	def cal_lin_vel(self, v, omega, w):

		v_rec = [0]*2
		tpdo = self.bus.recv(0.0)
		if tpdo is not None and tpdo.arbitration_id == 789: # 0x315 		
			t_s = tpdo.timestamp
			vd = tpdo.data
			v_rec[0] = vd[0] + (16**2)*vd[1] + (16**4)*vd[2] + (16**6)*vd[3]  			
			v_rec[1] = vd[4] + (16**2)*vd[5] + (16**4)*vd[6] + (16**6)*vd[7]  			
			for i in range(len(v_rec)):
				if v_rec[i] >= 2147483648:
					v_rec[i] = v_rec[i] - 4294967296
			v_lin_rec = self.convert_rpm_to_lin_vel(v_rec) 
			self.vel_dict.append({'timestamp' : t_s, 'v_lin_g' : v, 'v_lin_r' : (-v_lin_rec[0] + v_lin_rec[1])/2, 'v_ang_g' : omega, 'v_ang_r' : (v_lin_rec[1] + v_lin_rec[0])/w})
		return

	def convert_vel(self, v_lin):

		v_lin_conv = [0]*len(v_lin) 
		for i in range(len(v_lin)): 
			if v_lin[i] < 0: 					
				v_lin_conv[i] = -120*v_lin[i] 			
			else: 		
				v_lin_conv[i] = 120*v_lin[i]  		
		return v_lin_conv

	def move_rot(self, v, omega):

		w = 0.937
		v_lin = [v - w*omega/2, v + w*omega/2]  
		if v_lin[0] >= 0 and v_lin[1] >= 0:    
			a = 5
		elif v_lin[0] >= 0 and v_lin[1] < 0:
			a = 9
		elif v_lin[0] < 0 and v_lin[1] >= 0:
			a = 6
		else:
			a = 10 
		b = self.cal_hex(self.convert_vel(v_lin))
		self.bus.send(can.Message(arbitration_id = 405, data = [2, 0, 0, self.result, 0, 0, 0, 0], is_extended_id = False)) # 0x195	
		self.bus.send(can.Message(arbitration_id = 661, data = [16*a + 3, b[0][0], b[0][1], b[1][0], b[1][1], 1, 0, 0], is_extended_id = False)) # 0x295
		self.cal_lin_vel(v, omega, w)
		return  

	def plot_vel(self):

		t = []
		lin_vel_g = []
		lin_vel_r = []	
		ang_vel_g = []
		ang_vel_r = []	
		for i in range(len(self.vel_dict)):		
			t.append(self.vel_dict[i]['timestamp'] - self.vel_dict[0]['timestamp'])
			lin_vel_g.append(self.vel_dict[i]['v_lin_g'])
			lin_vel_r.append(self.vel_dict[i]['v_lin_r'])
			ang_vel_g.append(self.vel_dict[i]['v_ang_g'])
			ang_vel_r.append(self.vel_dict[i]['v_ang_r'])
		fig, (ax1, ax2) = plt.subplots(2)
		ax1.plot(t, lin_vel_g, label = "Given")
		ax1.plot(t, lin_vel_r, label = "Received")
		ax1.legend(fancybox=True)
		ax1.set_xlabel('Time(s)')
		ax1.set_ylabel('Linear velocity(m/s)') 		
		ax1.set_title('Linear velocity given v/s received')
		ax2.plot(t, ang_vel_g, label = "Given")
		ax2.plot(t, ang_vel_r, label = "Received")
		ax2.legend(fancybox=True)
		ax2.set_xlabel('Time(s)')
		ax2.set_ylabel('Angular velocity(rad/s)')
		ax2.set_title('Angular velocity given v/s received')
		plt.show()
		return 