import { Component, OnInit, ViewChild } from '@angular/core';
import { GraphComponent } from 'graph-lib';
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
      const values = [
        10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
        20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
        30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
        40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40,
        50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
        60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60];

      this.graph.graph.setWindowSize(1000, 500);
      const graphId = this.graph.graph.createGraph({
        valueCount: 1024,
        maxY: 50,
        width: 600,
        height: 75,
        origin: {
          x: 100,
          y: 50}});
      this.graph.graph.setGraphValues(graphId, 10, values);
    });
  }
}
