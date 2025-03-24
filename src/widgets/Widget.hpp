class Widget {
  virtual ~Widget();
  void RegisterDataCallback();

  virtual void ReceiveData();
  // Use ImGui to draw a this widget
  virtual void Draw();
};