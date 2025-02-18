#pragma once

namespace pruebas2 {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Resumen de window
	/// </summary>
	public ref class window : public System::Windows::Forms::Form
	{
	public:
		window(void)
		{
			InitializeComponent();
			//
			//TODO: agregar código de constructor aquí
			//
		}

		double expTime = 0.0;
		int height = 0;
		int width = 0;
		const char* ETH = "";

	private: System::Windows::Forms::Label^ label6;
	private: System::Windows::Forms::TextBox^ ETHBox;

	public:
		

		double GetExpTime() {
			return expTime;
		}

		int GetHeight() {
			return height;
		}

		int GetWidth() {
			return width;
		}

		const char* GetETH() {
			return ETH;
		}

		

	protected:
		/// <summary>
		/// Limpiar los recursos que se estén usando.
		/// </summary>
		~window()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Label^ label1;
	private: System::Windows::Forms::Button^ button1;
	private: System::Windows::Forms::Label^ label2;
	private: System::Windows::Forms::Label^ label4;
	private: System::Windows::Forms::TextBox^ widthBox;
	private: System::Windows::Forms::Label^ label3;
	private: System::Windows::Forms::TextBox^ heightBox;
	private: System::Windows::Forms::Label^ label5;
	private: System::Windows::Forms::TextBox^ expBox;
	protected:

	private:
		/// <summary>
		/// Variable del diseñador necesaria.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Método necesario para admitir el Diseñador. No se puede modificar
		/// el contenido de este método con el editor de código.
		/// </summary>
		void InitializeComponent(void)
		{
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->widthBox = (gcnew System::Windows::Forms::TextBox());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->heightBox = (gcnew System::Windows::Forms::TextBox());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->expBox = (gcnew System::Windows::Forms::TextBox());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->ETHBox = (gcnew System::Windows::Forms::TextBox());
			this->SuspendLayout();
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 20));
			this->label1->Location = System::Drawing::Point(14, 9);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(379, 39);
			this->label1->TabIndex = 0;
			this->label1->Text = L"Parameter configuration";
			// 
			// button1
			// 
			this->button1->Location = System::Drawing::Point(256, 249);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(75, 23);
			this->button1->TabIndex = 1;
			this->button1->Text = L"Enter";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &window::button1_Click);
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label2->Location = System::Drawing::Point(52, 153);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(88, 20);
			this->label2->TabIndex = 2;
			this->label2->Text = L"Resolution";
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(51, 192);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(41, 16);
			this->label4->TabIndex = 4;
			this->label4->Text = L"Width";
			// 
			// widthBox
			// 
			this->widthBox->Location = System::Drawing::Point(103, 189);
			this->widthBox->Name = L"widthBox";
			this->widthBox->Size = System::Drawing::Size(100, 22);
			this->widthBox->TabIndex = 6;
			this->widthBox->Text = L"1392";
			this->widthBox->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(51, 228);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(46, 16);
			this->label3->TabIndex = 7;
			this->label3->Text = L"Height";
			// 
			// heightBox
			// 
			this->heightBox->Location = System::Drawing::Point(103, 228);
			this->heightBox->Name = L"heightBox";
			this->heightBox->Size = System::Drawing::Size(100, 22);
			this->heightBox->TabIndex = 8;
			this->heightBox->Text = L"1040";
			this->heightBox->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label5->Location = System::Drawing::Point(52, 83);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(121, 20);
			this->label5->TabIndex = 9;
			this->label5->Text = L"Exposure Time";
			// 
			// expBox
			// 
			this->expBox->Location = System::Drawing::Point(231, 83);
			this->expBox->Name = L"expBox";
			this->expBox->Size = System::Drawing::Size(100, 22);
			this->expBox->TabIndex = 10;
			this->expBox->Text = L"5";
			this->expBox->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10.2F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label6->Location = System::Drawing::Point(50, 117);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(155, 20);
			this->label6->TabIndex = 11;
			this->label6->Text = L"ETH_ADAPTER_IP";
			// 
			// ETHBox
			// 
			this->ETHBox->Location = System::Drawing::Point(231, 117);
			this->ETHBox->Name = L"ETHBox";
			this->ETHBox->Size = System::Drawing::Size(100, 22);
			this->ETHBox->TabIndex = 12;
			this->ETHBox->Text = L"192.168.1.100";
			this->ETHBox->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// window
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(444, 305);
			this->Controls->Add(this->ETHBox);
			this->Controls->Add(this->label6);
			this->Controls->Add(this->expBox);
			this->Controls->Add(this->label5);
			this->Controls->Add(this->heightBox);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->widthBox);
			this->Controls->Add(this->label4);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->button1);
			this->Controls->Add(this->label1);
			this->Name = L"window";
			this->Text = L"window";
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void button1_Click(System::Object^ sender, System::EventArgs^ e) {
		try {
			expTime = System::Convert::ToDouble(expBox->Text);
			height = System::Convert::ToInt32(heightBox->Text);
			width = System::Convert::ToInt32(widthBox->Text);
			ETH = (const char*)(System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(ETHBox->Text)).ToPointer();

			if (expTime <= 0 || height <= 0 || width <= 0) {
				throw gcnew System::Exception("Values must be greater than zero.");
			}

			this->Close();
		}
		catch (System::Exception^ ex) {
			MessageBox::Show("Error: " + ex->Message, "Input Validation Error");
		}


	}
};
}
