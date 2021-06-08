import { Component, OnInit, ViewChild } from '@angular/core';
import { GraphComponent } from 'graph-lib';
import { interval } from 'rxjs';
import { filter, take } from 'rxjs/operators';

@Component({
  selector: 'app-root',
  template: `
    <lib-webgl-graph [wasmAssetPath]="'assets/wasm/graph.wasm'"></lib-webgl-graph>
  `,
  styles: []
})
export class AppComponent implements OnInit {
  @ViewChild(GraphComponent, {static: true}) graph: GraphComponent;

  ngOnInit(): void {
    this.graph.isRuntimeInitialized.pipe(filter((it) => it), take(1)).subscribe(() => {
      const alarm_levels = [
        "unknown",
        "ok",
        "warn",
        "error"
      ];
      let values = [];
      let new_alarm_counter = 0;
      let new_alarm = "unknown";
      for (let i = 0; i < 1024 * 3000; i++) {
        values[i] = Math.random() + (Math.sin((i + Math.sin(i / 8192) * 5) / 256 * Math.PI) + 1) * 10 * (i/ 300000);
        if(!(i % 1024)){
          if(new_alarm_counter <= 0){
            new_alarm_counter = Math.round(Math.random() * 40 + 10);
            new_alarm = alarm_levels[Math.round(Math.random() * 3)];
            console.log(i, new_alarm);
          }
          new_alarm_counter--;
          this.graph.graph.addSpectrumMetadata({
            reference_key: i % 2048 ? 'a' : 'b',
            timestamp: (new Date).toLocaleString().split(' ')[1],
            part_number: Math.round(i / 1024 / 200),
            alarm_level: new_alarm,
            workstep: Math.round(i / 1024 / 40) % 40
          });
        }
      }
      this.graph.graph.addSpectrenValues(values);
      values = [];
      interval(200).pipe(take(25)).subscribe(() => {
        for (let i = 0; i < 1024 * 5; i++) {
          values[i] = Math.random() + (Math.sin((i + Math.cos(i / 8192) * 5) / 256 * Math.PI) + 1) * 100;
        }
        this.graph.graph.addSpectrenValues(values);
      });
      const reference = [];
      for (let i = 0; i < 1024; i++) {
        reference[i] = (i % 100) / 10;
      }
      this.graph.graph.addReference('a', reference);

    });
  }
}
