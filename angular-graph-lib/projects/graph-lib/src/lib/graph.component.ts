import { AfterViewInit, Component, ElementRef, Input, Output, ViewChild } from '@angular/core';
import { BehaviorSubject, combineLatest } from 'rxjs';
import * as Graph from './angular_graph.js';
import { WebGLGraph } from './graph.model.js';
import { filter, take } from 'rxjs/operators';

@Component({
  selector: 'lib-webgl-graph',
  templateUrl: './graph.component.html',
  styleUrls: ['./graph.component.scss']
})
export class GraphComponent implements AfterViewInit {
  @Input() set wasmAssetPath(path: string){
    this._wasmAssetPath.next(path);
  }
  @Output() state: BehaviorSubject<string> = new BehaviorSubject('');
  @Output() loading = true;
  @Output() isRuntimeInitialized: BehaviorSubject<boolean> = new BehaviorSubject(false);
  @Output() public get graph(): WebGLGraph {
    return this._graph;
  }
  private _graph: any;

  private _wasmAssetPath = new BehaviorSubject('');
  private _afterViewInit = new BehaviorSubject(false);

  @ViewChild('graphCanvas', {static: true}) private canvas: ElementRef<HTMLCanvasElement>;

  constructor() {
    combineLatest([
      this._wasmAssetPath.pipe(filter((it) => !!it)),
      this._afterViewInit.pipe(filter((it) => it))])
      .pipe(take(1))
      .subscribe(([path, _]) => {
        this.initGraph(path);
      });
  }

  ngAfterViewInit(): void {
    this._afterViewInit.next(true);
  }

  private initGraph(path: string): void {
    this._graph = {
      preRun: [],
      postRun: [],
      print: (() => {
        return (text: string) => {
          if (arguments.length > 1) { text = Array.prototype.slice.call(arguments).join(' '); }
          console.log(text);
        };
      })(),
      printErr: (text: string) => {
        if (arguments.length > 1) { text = Array.prototype.slice.call(arguments).join(' '); }
        console.error(text);
      },
      canvas: ( () => {
        this.canvas.nativeElement.addEventListener('webglcontextlost', (e) => {
          alert('WebGL context lost. You will need to reload the page.');
          e.preventDefault();
        }, false);
        return this.canvas.nativeElement;
      })(),
      setStatus: (text: string) => {
        if (!this._graph.setStatus.last) {
          this._graph.setStatus.last = { text: '' };
        }
        if (text === this._graph.setStatus.last.text) {
          return;
        }
        this.state.next(text);
        const m = text.match(/([^(]+)\((\d+(\.\d+)?)\/(\d+)\)/);
        this._graph.setStatus.last.text = text;
        if (m) {
          this.loading = true;
        } else {
          if (!text) { this.loading = false; }
        }
      },
      onRuntimeInitialized: () => {

        this._graph.setWindowSize = (w: number, h: number) => {
          this._graph.setCanvasSize(w, h, true);
          this._graph._setWindowSize(w, h);
        };

        this._graph.setGraphValues = (id: number, offset: number, values: number[]): boolean => {
          const ptr = this._graph._malloc(8 * values.length);
          this._graph.HEAPF32.set(new Float32Array(values), ptr / 4);
          const result = this._graph._setGraphValues(id, offset, ptr, values.length);
          this._graph._free(ptr);
          return result;
        };
        setTimeout(() => this.isRuntimeInitialized.next(true), 0);
      }
    };
    Graph.init(this._graph, path);
  }

}
