Bước 1:
	Hiểu về mặt phần cứng của peripheral mà mình cần viết driver
Bước 2:
	Tìm hiểu chuẩn giao tiếp giữa application của Linux với pheripheral của mình
Bước 3:
	Tìm hiểu template - example code
Bước 4:
	Viết driver
	
Bài toán
	Cắm đèn led vào board qua chân gpio
	Tạo ra 1 device file tên là /dev/led_example, device file được dùng để giao tiếp vs application
	Cho phép application có thể đọc / ghi dữ liệu vào đèn led thông qua device file.
	Theo chuẩn như sau:
		Nếu application ghi "1" vào device file -> led bật sáng
		Nếu application ghi "0" vào device file -> led tắt
		Nếu led đang tắt mà application đọc từ device file, thì dữ liệu đọc về là "0"
		Nếu led đang bật mà application đọc từ device file, thì dữ liệu đọc về là "1"
		Tính năng config
			App có thể config số lần nháy đèn thông qua ioctl
			App cũng có thể lấy về config số lần nháy đèn thông qua ioctl
			Ví dụ:
				Nếu config số lần nháy là 10, thì khi ghi 1 vào device file, đèn
				sẽ nháy 10 lần
		
device driver / protocol driver
	Driver thao tac voi device duoc ket noi vao SoC
	Driver thao tac voi peripheral thong qua API cua platform driver
platform driver / controller driver
	Driver thao tac truc tiep voi thanh ghi cua module do
	trong SoC
	Driver cung cap API cho kernel va cac device driver khac su dung

Magic number: 100 
Size of data: 4 
Type of data: 15 

32 bits luu tru 3 thong tin

8 bit dau luu tru Magic number
11 bit giua luu tru size of data
13 bit cuoi de luu tru type of data


//struct bị quy định sẵn
struct hard_code
{
	int a;
	int b;
}

void timer(struct hard_code *timer_data)
{
	struct container *my_data = container_of(timer_data, struct container, hard_code);
	char c = my_data.c;
}

struct container {
	char c;
	struct hard_code timer_data;
};

int main()
{
	struct container my_data;
	my_data.c = 6;
	timer(&(my_data.timer_data));
}


result = request_irq(irqNumber,               // The interrupt number requested
         (irq_handler_t) ebbgpio_irq_handler, // The pointer to the handler function (above)
         IRQF_TRIGGER_RISING,                 // Interrupt is on rising edge (button press in Fig.1)
         "ebb_gpio_handler",                  // Used in /proc/interrupts to identify the owner
         NULL);                               // The *dev_id for shared interrupt lines, NULL here