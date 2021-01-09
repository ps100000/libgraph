import { BrowserModule } from '@angular/platform-browser';
import { NgModule } from '@angular/core';

import { AppComponent } from './app.component';
import { GraphLibModule } from 'graph-lib';

@NgModule({
  declarations: [
    AppComponent
  ],
  imports: [
    BrowserModule,
    GraphLibModule
  ],
  providers: [],
  bootstrap: [AppComponent]
})
export class AppModule { }
